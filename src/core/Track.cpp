#include "core/Track.h"
#include <algorithm>

namespace ds::core {

Track::Track(std::string id, TrackType type, int index)
    : id_(std::move(id)), type_(type), index_(index) {}

Clip* Track::addClip(std::unique_ptr<Clip> clip) {
    Clip* raw = clip.get();
    clips_.push_back(std::move(clip));
    std::sort(clips_.begin(), clips_.end(), [](const auto& a, const auto& b) {
        return a->timelineStart() < b->timelineStart();
    });
    return raw;
}

bool Track::removeClip(const std::string& clipId) {
    auto it = std::find_if(clips_.begin(), clips_.end(),
        [&](const auto& c) { return c->id() == clipId; });
    if (it == clips_.end()) return false;
    clips_.erase(it);
    if (magnetic_) closeGaps();
    return true;
}

Clip* Track::clipAt(FrameTime frame) const {
    for (const auto& c : clips_) {
        if (frame >= c->timelineStart() && frame < c->timelineEnd()) {
            return c.get();
        }
    }
    return nullptr;
}

void Track::closeGaps() {
    FrameTime cursor = 0;
    for (auto& c : clips_) {
        c->moveTo(cursor);
        cursor += c->duration();
    }
}

} // namespace ds::core
