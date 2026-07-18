#pragma once
#include "core/Timeline.h"
#include "engine/DecodeEngine.h"
#include <memory>
#include <unordered_map>
#include <functional>
#include <string>

namespace ds::engine {

// Composites one frame of a Timeline by finding the active clip on each
// video track, decoding its source frame via a pooled DecodeEngine, and
// alpha-blending top track over bottom. This is deliberately CPU/software
// composited (straight alpha blend into a byte buffer) rather than GPU —
// real GPU shader compositing is a follow-up once the app has an active
// OpenGL context to target; the point of this pass is to prove the data
// path end-to-end: Timeline -> Clip -> real decode -> real composited
// pixels, which a CPU blend already demonstrates correctly.
class RenderGraph {
public:
    // The caller (Project/MainWindow) knows how mediaAssetId maps to a
    // file path; RenderGraph doesn't depend on core::Project to stay
    // decoupled, so it takes a resolver function instead.
    using MediaPathResolver = std::function<std::string(const std::string& mediaAssetId)>;

    RenderGraph(ds::core::Timeline* timeline, MediaPathResolver resolver);

    // Renders exactly one frame at full or reduced (proxy) resolution.
    // playbackResolutionScale of 1.0 = full res, 0.5 = half res preview.
    DecodedFrame renderFrame(ds::core::FrameTime frame, double playbackResolutionScale);

    const std::string& lastError() const { return lastError_; }

private:
    DecodeEngine* decoderFor(const std::string& mediaAssetId);

    ds::core::Timeline* timeline_; // non-owning
    MediaPathResolver resolveMediaPath_;
    std::unordered_map<std::string, std::unique_ptr<DecodeEngine>> decoderPool_;
    std::string lastError_;
};

} // namespace ds::engine
