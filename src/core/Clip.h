#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include <vector>

namespace ds::core {

// Frame-accurate time. All editing math happens in integer frames at the
// project's frame rate to avoid float drift on long timelines.
using FrameTime = int64_t;

enum class ClipType { Video, Audio, Image, Title, AdjustmentLayer, CompoundClip };

struct TrimHandles {
    FrameTime inPoint  = 0; // first visible source frame
    FrameTime outPoint = 0; // last visible source frame (inclusive)
};

// A single effect/filter instance applied to a clip (color, blur, etc).
struct EffectInstance {
    std::string effectId;                 // registry key, e.g. "color.curves"
    bool enabled = true;
    std::vector<std::pair<std::string, double>> params; // param name -> value
    // Keyframes are stored per-param in a separate KeyframeTrack (see Keyframe.h)
};

class Clip {
public:
    Clip(std::string id, ClipType type, std::string mediaAssetId);

    const std::string& id() const { return id_; }
    ClipType type() const { return type_; }
    const std::string& mediaAssetId() const { return mediaAssetId_; }

    // Position on the track, in project frames.
    FrameTime timelineStart() const { return timelineStart_; }
    FrameTime timelineEnd() const { return timelineStart_ + duration(); }
    FrameTime duration() const { return trim_.outPoint - trim_.inPoint + 1; }

    void moveTo(FrameTime newStart) { timelineStart_ = newStart; }
    void setTrim(TrimHandles t) { trim_ = t; }
    const TrimHandles& trim() const { return trim_; }

    // Ripple/roll/slip/slide editing primitives operate on these two calls.
    void slip(FrameTime deltaFrames);   // shift source in/out together
    void trimIn(FrameTime newIn);       // roll left edge
    void trimOut(FrameTime newOut);     // roll right edge

    std::vector<EffectInstance>& effects() { return effects_; }
    const std::vector<EffectInstance>& effects() const { return effects_; }

    bool locked() const { return locked_; }
    void setLocked(bool v) { locked_ = v; }

private:
    std::string id_;
    ClipType type_;
    std::string mediaAssetId_;
    FrameTime timelineStart_ = 0;
    TrimHandles trim_;
    std::vector<EffectInstance> effects_;
    bool locked_ = false;
};

} // namespace ds::core
