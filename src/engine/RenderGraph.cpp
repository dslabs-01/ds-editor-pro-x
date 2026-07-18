extern "C" {
#include <libswscale/swscale.h>
}
#include "engine/RenderGraph.h"

namespace ds::engine {

RenderGraph::RenderGraph(ds::core::Timeline* timeline, MediaPathResolver resolver)
    : timeline_(timeline), resolveMediaPath_(std::move(resolver)) {}

DecodeEngine* RenderGraph::decoderFor(const std::string& mediaAssetId) {
    auto it = decoderPool_.find(mediaAssetId);
    if (it != decoderPool_.end()) return it->second.get();

    std::string path = resolveMediaPath_(mediaAssetId);
    if (path.empty()) {
        lastError_ = "no media path resolved for asset " + mediaAssetId;
        return nullptr;
    }

    auto engine = std::make_unique<DecodeEngine>(path);
    if (!engine->open()) {
        lastError_ = "failed to open " + path + ": " + engine->lastError();
        return nullptr;
    }
    DecodeEngine* raw = engine.get();
    decoderPool_[mediaAssetId] = std::move(engine);
    return raw;
}

// Simple nearest-quality resize via swscale — used when a decoded frame's
// native resolution doesn't match the requested composite resolution
// (playback proxy scale, or a clip whose source resolution differs from
// the project's timeline resolution).
static void resizeRGBA(const DecodedFrame& src, DecodedFrame& dst, int targetW, int targetH) {
    if (src.width == targetW && src.height == targetH) {
        dst = src;
        return;
    }
    SwsContext* sws = sws_getContext(
        src.width, src.height, AV_PIX_FMT_RGBA,
        targetW, targetH, AV_PIX_FMT_RGBA,
        SWS_BILINEAR, nullptr, nullptr, nullptr);

    dst.width = targetW;
    dst.height = targetH;
    dst.data.resize(static_cast<size_t>(targetW) * targetH * 4);

    const uint8_t* srcData[1] = { src.data.data() };
    int srcLinesize[1] = { src.width * 4 };
    uint8_t* dstData[1] = { dst.data.data() };
    int dstLinesize[1] = { targetW * 4 };

    sws_scale(sws, srcData, srcLinesize, 0, src.height, dstData, dstLinesize);
    sws_freeContext(sws);
}

// Straight alpha-over blend: dst = src*srcA + dst*(1-srcA), per pixel.
static void alphaOverComposite(DecodedFrame& base, const DecodedFrame& top) {
    size_t n = std::min(base.data.size(), top.data.size());
    for (size_t i = 0; i + 3 < n; i += 4) {
        float a = top.data[i + 3] / 255.0f;
        base.data[i + 0] = static_cast<uint8_t>(top.data[i + 0] * a + base.data[i + 0] * (1 - a));
        base.data[i + 1] = static_cast<uint8_t>(top.data[i + 1] * a + base.data[i + 1] * (1 - a));
        base.data[i + 2] = static_cast<uint8_t>(top.data[i + 2] * a + base.data[i + 2] * (1 - a));
        base.data[i + 3] = static_cast<uint8_t>(std::max<int>(base.data[i + 3], top.data[i + 3]));
    }
}

DecodedFrame RenderGraph::renderFrame(ds::core::FrameTime frame, double playbackResolutionScale) {
    DecodedFrame composite;
    composite.frameNumber = frame;
    composite.width  = std::max(1, static_cast<int>(timeline_->width()  * playbackResolutionScale));
    composite.height = std::max(1, static_cast<int>(timeline_->height() * playbackResolutionScale));
    composite.data.assign(static_cast<size_t>(composite.width) * composite.height * 4, 0);
    // Alpha starts at 0 (nothing composited yet = fully transparent /
    // shows as black in a plain RGB viewer, matching "no content on any
    // track at this frame").

    for (const auto& track : timeline_->tracks()) {
        if (track->type() != ds::core::TrackType::Video) continue;
        if (track->muted()) continue;

        ds::core::Clip* clip = track->clipAt(frame);
        if (!clip) continue;

        DecodeEngine* decoder = decoderFor(clip->mediaAssetId());
        if (!decoder) continue; // lastError_ already set by decoderFor

        ds::core::FrameTime sourceFrame =
            (frame - clip->timelineStart()) + clip->trim().inPoint;

        DecodedFrame decoded;
        if (!decoder->decodeFrame(sourceFrame, decoded)) {
            lastError_ = "decode failed for clip " + clip->id() + " at source frame "
                       + std::to_string(sourceFrame) + ": " + decoder->lastError();
            continue;
        }

        // TODO: apply clip->effects() chain (color/VFX) to `decoded` here,
        // before compositing, once ColorPipeline::applyToFrame is wired to
        // real OCIO calls instead of being a no-op stub.

        DecodedFrame resized;
        resizeRGBA(decoded, resized, composite.width, composite.height);
        alphaOverComposite(composite, resized);
    }

    return composite;
}

} // namespace ds::engine
