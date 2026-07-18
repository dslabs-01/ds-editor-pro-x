#pragma once
#include "core/Clip.h"
#include <string>
#include <vector>
#include <cstdint>

struct AVFormatContext;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
struct SwsContext;

namespace ds::engine {

struct DecodedFrame {
    int width = 0;
    int height = 0;
    std::string pixelFormat = "rgba"; // output always normalized to RGBA
    std::vector<uint8_t> data;        // width*height*4 bytes, row-major
    ds::core::FrameTime frameNumber = 0;
    double ptsSeconds = 0.0;
    bool isProxy = false;
};

// Real libavformat/libavcodec-backed decoder. One instance per open media
// file. decodeFrame() seeks to the nearest keyframe at or before the
// requested frame, then decodes forward until it reaches it, converting
// the result to RGBA via libswscale.
class DecodeEngine {
public:
    explicit DecodeEngine(std::string sourcePath);
    ~DecodeEngine();

    DecodeEngine(const DecodeEngine&) = delete;
    DecodeEngine& operator=(const DecodeEngine&) = delete;

    bool open();
    void close();
    bool isOpen() const { return opened_; }

    int width() const { return width_; }
    int height() const { return height_; }
    double frameRate() const { return frameRate_; }
    int64_t totalFrames() const { return totalFrames_; }

    bool decodeFrame(ds::core::FrameTime frame, DecodedFrame& out);

    const std::string& lastError() const { return lastError_; }

private:
    bool decodeNextFrameInternal(AVFrame* frame);
    void convertToRGBA(AVFrame* frame, DecodedFrame& out);

    std::string sourcePath_;
    bool opened_ = false;
    std::string lastError_;

    AVFormatContext* formatCtx_ = nullptr;
    AVCodecContext* codecCtx_ = nullptr;
    SwsContext* swsCtx_ = nullptr;
    int videoStreamIndex_ = -1;

    int width_ = 0;
    int height_ = 0;
    double frameRate_ = 0.0;
    int64_t totalFrames_ = 0;
    ds::core::FrameTime lastDecodedFrame_ = -1;
    bool eofFlushStarted_ = false;
};

} // namespace ds::engine
