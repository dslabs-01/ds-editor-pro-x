#pragma once
#include "core/Track.h"
#include <vector>
#include <memory>
#include <map>
#include <string>

namespace ds::core {

struct Marker {
    FrameTime frame;
    std::string label;
    std::string colorHex = "#3B82F6";
};

// A Timeline is a full editable sequence: tracks + markers + settings.
// Nested timelines / compound clips are represented by a Timeline being
// referenceable as a MediaAsset elsewhere (see MediaAsset.h), which is how
// "nested timelines" and "compound clips" share one implementation.
class Timeline {
public:
    Timeline(std::string id, std::string name, int fpsNumerator, int fpsDenominator,
              int width, int height);

    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    double frameRate() const { return static_cast<double>(fpsNum_) / fpsDen_; }
    int width() const { return width_; }
    int height() const { return height_; }

    Track* addTrack(TrackType type);
    bool removeTrack(const std::string& trackId);
    const std::vector<std::unique_ptr<Track>>& tracks() const { return tracks_; }

    void addMarker(Marker m) { markers_.push_back(std::move(m)); }
    const std::vector<Marker>& markers() const { return markers_; }

    FrameTime durationFrames() const;

    // Playhead / selection are UI-facing but tracked here so any panel
    // (viewer, timeline, scopes) can stay in sync without direct coupling.
    FrameTime playhead() const { return playhead_; }
    void setPlayhead(FrameTime f) { playhead_ = f; }

private:
    std::string id_;
    std::string name_;
    int fpsNum_, fpsDen_;
    int width_, height_;
    std::vector<std::unique_ptr<Track>> tracks_;
    std::vector<Marker> markers_;
    FrameTime playhead_ = 0;
    int nextTrackIndex_ = 0;
};

} // namespace ds::core
