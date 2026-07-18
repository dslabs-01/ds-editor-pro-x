#include "core/Clip.h"
#include <algorithm>

namespace ds::core {

Clip::Clip(std::string id, ClipType type, std::string mediaAssetId)
    : id_(std::move(id)), type_(type), mediaAssetId_(std::move(mediaAssetId)) {}

void Clip::slip(FrameTime deltaFrames) {
    // Slip: move the in/out window over the source media without changing
    // timeline position or duration.
    trim_.inPoint  += deltaFrames;
    trim_.outPoint += deltaFrames;
}

void Clip::trimIn(FrameTime newIn) {
    // Roll edit on the left edge: duration changes, timeline position of the
    // *right* edge stays fixed by construction (caller repositions neighbor).
    newIn = std::min(newIn, trim_.outPoint - 1); // keep at least 1 frame
    FrameTime shift = newIn - trim_.inPoint;
    trim_.inPoint = newIn;
    timelineStart_ += shift;
}

void Clip::trimOut(FrameTime newOut) {
    trim_.outPoint = std::max(newOut, trim_.inPoint + 1);
}

} // namespace ds::core
