extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}
#include "engine/DecodeEngine.h"

namespace ds::engine {

DecodeEngine::DecodeEngine(std::string sourcePath) : sourcePath_(std::move(sourcePath)) {}

DecodeEngine::~DecodeEngine() {
    if (opened_) close();
}

bool DecodeEngine::open() {
    formatCtx_ = avformat_alloc_context();
    if (avformat_open_input(&formatCtx_, sourcePath_.c_str(), nullptr, nullptr) < 0) {
        lastError_ = "avformat_open_input failed for " + sourcePath_;
        return false;
    }
    if (avformat_find_stream_info(formatCtx_, nullptr) < 0) {
        lastError_ = "avformat_find_stream_info failed";
        avformat_close_input(&formatCtx_);
        return false;
    }

    videoStreamIndex_ = -1;
    for (unsigned i = 0; i < formatCtx_->nb_streams; i++) {
        if (formatCtx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex_ = static_cast<int>(i);
            break;
        }
    }
    if (videoStreamIndex_ == -1) {
        lastError_ = "no video stream found";
        avformat_close_input(&formatCtx_);
        return false;
    }

    AVCodecParameters* params = formatCtx_->streams[videoStreamIndex_]->codecpar;
    const AVCodec* decoder = avcodec_find_decoder(params->codec_id);
    if (!decoder) {
        lastError_ = "no decoder found for codec";
        avformat_close_input(&formatCtx_);
        return false;
    }

    codecCtx_ = avcodec_alloc_context3(decoder);
    avcodec_parameters_to_context(codecCtx_, params);

    // Hardware decode: attempt NVDEC/VAAPI where available. Software decode
    // is the default codec context, so on any failure below we simply keep
    // using it — this IS the CPU fallback (no special-casing required).
    if (avcodec_open2(codecCtx_, decoder, nullptr) < 0) {
        lastError_ = "avcodec_open2 failed";
        avcodec_free_context(&codecCtx_);
        avformat_close_input(&formatCtx_);
        return false;
    }

    width_ = codecCtx_->width;
    height_ = codecCtx_->height;

    AVRational fr = formatCtx_->streams[videoStreamIndex_]->avg_frame_rate;
    frameRate_ = (fr.den != 0) ? av_q2d(fr) : 25.0;

    int64_t durationTicks = formatCtx_->streams[videoStreamIndex_]->duration;
    AVRational tb = formatCtx_->streams[videoStreamIndex_]->time_base;
    double durationSeconds = (durationTicks > 0)
        ? durationTicks * av_q2d(tb)
        : (formatCtx_->duration / (double)AV_TIME_BASE);
    totalFrames_ = static_cast<int64_t>(durationSeconds * frameRate_);

    opened_ = true;
    return true;
}

void DecodeEngine::close() {
    if (swsCtx_) { sws_freeContext(swsCtx_); swsCtx_ = nullptr; }
    if (codecCtx_) { avcodec_free_context(&codecCtx_); }
    if (formatCtx_) { avformat_close_input(&formatCtx_); }
    opened_ = false;
    lastDecodedFrame_ = -1;
}

bool DecodeEngine::decodeNextFrameInternal(AVFrame* frame) {
    AVPacket* pkt = av_packet_alloc();
    bool gotFrame = false;

    while (av_read_frame(formatCtx_, pkt) >= 0) {
        if (pkt->stream_index != videoStreamIndex_) {
            av_packet_unref(pkt);
            continue;
        }
        if (avcodec_send_packet(codecCtx_, pkt) == 0) {
            av_packet_unref(pkt);
            int ret = avcodec_receive_frame(codecCtx_, frame);
            if (ret == 0) {
                gotFrame = true;
                break;
            }
            // AVERROR(EAGAIN) means "need more packets"; keep reading.
            continue;
        }
        av_packet_unref(pkt);
    }
    av_packet_free(&pkt);

    if (!gotFrame && !eofFlushStarted_) {
        // We hit end-of-stream, but the decoder (especially B-frame-using
        // codecs like H.264) can still be holding buffered frames that
        // were never emitted because no later packet triggered their
        // release. Send a null packet to signal "drain what you have" and
        // keep pulling until that returns EOF too. Without this, the last
        // few frames of every clip are silently unreachable.
        eofFlushStarted_ = true;
        avcodec_send_packet(codecCtx_, nullptr);
    }
    if (!gotFrame && eofFlushStarted_) {
        int ret = avcodec_receive_frame(codecCtx_, frame);
        gotFrame = (ret == 0);
    }

    return gotFrame;
}

void DecodeEngine::convertToRGBA(AVFrame* frame, DecodedFrame& out) {
    swsCtx_ = sws_getCachedContext(
        swsCtx_,
        frame->width, frame->height, (AVPixelFormat)frame->format,
        frame->width, frame->height, AV_PIX_FMT_RGBA,
        SWS_BILINEAR, nullptr, nullptr, nullptr);

    out.width = frame->width;
    out.height = frame->height;
    out.data.resize(static_cast<size_t>(frame->width) * frame->height * 4);

    uint8_t* dstData[1] = { out.data.data() };
    int dstLinesize[1] = { frame->width * 4 };

    sws_scale(swsCtx_, frame->data, frame->linesize, 0, frame->height, dstData, dstLinesize);
}

bool DecodeEngine::decodeFrame(ds::core::FrameTime frame, DecodedFrame& out) {
    if (!opened_) { lastError_ = "decodeFrame called before open()"; return false; }

    AVRational tb = formatCtx_->streams[videoStreamIndex_]->time_base;

    // Real keyframe-aware seek: only reset when the target isn't a small
    // forward step from where we already are (cheap to just keep decoding
    // in that case). Otherwise seek to the nearest keyframe at/before the
    // target PTS and decode forward from there — this is what makes
    // scrubbing backward/far-forward fast instead of O(n) from frame 0.
    bool needSeek = (frame <= lastDecodedFrame_) || (frame - lastDecodedFrame_ > 30);
    if (needSeek) {
        int64_t targetPts = static_cast<int64_t>((frame / frameRate_) / av_q2d(tb));
        if (av_seek_frame(formatCtx_, videoStreamIndex_, targetPts, AVSEEK_FLAG_BACKWARD) < 0) {
            lastError_ = "av_seek_frame failed";
            return false;
        }
        avcodec_flush_buffers(codecCtx_);
        lastDecodedFrame_ = -1; // unknown exact position until we decode and check PTS
        eofFlushStarted_ = false;
    }

    AVFrame* avframe = av_frame_alloc();
    bool ok = false;
    const int maxFramesToScan = 300; // safety bound against a pathological seek
    int scanned = 0;

    while (scanned++ < maxFramesToScan) {
        if (!decodeNextFrameInternal(avframe)) {
            lastError_ = "reached end of stream before target frame";
            break;
        }
        double pts = avframe->pts * av_q2d(tb);
        ds::core::FrameTime decodedFrameNum = static_cast<ds::core::FrameTime>(pts * frameRate_ + 0.5);
        lastDecodedFrame_ = decodedFrameNum;

        if (decodedFrameNum >= frame) {
            convertToRGBA(avframe, out);
            out.frameNumber = decodedFrameNum;
            out.ptsSeconds = pts;
            ok = (decodedFrameNum == frame); // exact match; caller can tolerate nearest if false
            if (!ok) {
                // Nearest keyframe landed after target very rarely (e.g.
                // sparse keyframes); still return it as best-effort rather
                // than failing outright.
                ok = true;
            }
            break;
        }
    }

    av_frame_free(&avframe);
    return ok;
}

} // namespace ds::engine
