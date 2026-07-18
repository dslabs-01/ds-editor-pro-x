#include "core/Timeline.h"
#include <algorithm>
#include <sstream>

namespace ds::core {

Timeline::Timeline(std::string id, std::string name, int fpsNumerator, int fpsDenominator,
                      int width, int height)
    : id_(std::move(id)), name_(std::move(name)),
      fpsNum_(fpsNumerator), fpsDen_(fpsDenominator),
      width_(width), height_(height) {}

Track* Timeline::addTrack(TrackType type) {
    std::ostringstream idStream;
    idStream << "track_" << nextTrackIndex_;
    auto track = std::make_unique<Track>(idStream.str(), type, nextTrackIndex_);
    Track* raw = track.get();
    tracks_.push_back(std::move(track));
    nextTrackIndex_++;
    return raw;
}

bool Timeline::removeTrack(const std::string& trackId) {
    auto it = std::find_if(tracks_.begin(), tracks_.end(),
        [&](const auto& t) { return t->id() == trackId; });
    if (it == tracks_.end()) return false;
    tracks_.erase(it);
    return true;
}

FrameTime Timeline::durationFrames() const {
    FrameTime maxEnd = 0;
    for (const auto& track : tracks_) {
        for (const auto& clip : track->clips()) {
            maxEnd = std::max(maxEnd, clip->timelineEnd());
        }
    }
    return maxEnd;
}

} // namespace ds::core
