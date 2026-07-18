#pragma once
#include "core/Timeline.h"
#include "core/MediaAsset.h"
#include <unordered_map>
#include <memory>
#include <string>

namespace ds::core {

// A Project is the .dsproj file's in-memory representation: media bin +
// one or more timelines (sequences) + project-wide settings. Kept engine-
// and UI-agnostic so it can be unit tested and serialized independently.
class Project {
public:
    explicit Project(std::string name);

    const std::string& name() const { return name_; }
    void rename(std::string n) { name_ = std::move(n); }

    MediaAsset* importMedia(const std::string& path, MediaKind kind);
    MediaAsset* findMedia(const std::string& id) const;

    Timeline* createTimeline(const std::string& name, int fpsNum, int fpsDen,
                               int width, int height);
    Timeline* findTimeline(const std::string& id) const;
    const std::unordered_map<std::string, std::unique_ptr<Timeline>>& timelines() const {
        return timelines_;
    }

    // Autosave interval in seconds; 0 disables. Wired up by
    // engine/PlaybackController's owning app loop, not this class.
    int autosaveIntervalSeconds = 120;

private:
    std::string name_;
    std::unordered_map<std::string, std::unique_ptr<MediaAsset>> mediaBin_;
    std::unordered_map<std::string, std::unique_ptr<Timeline>> timelines_;
    int nextMediaId_ = 0;
    int nextTimelineId_ = 0;
};

} // namespace ds::core
