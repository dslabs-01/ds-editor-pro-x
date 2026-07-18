#include "core/Project.h"
#include <sstream>

namespace ds::core {

Project::Project(std::string name) : name_(std::move(name)) {}

MediaAsset* Project::importMedia(const std::string& path, MediaKind kind) {
    std::ostringstream idStream;
    idStream << "media_" << nextMediaId_++;
    auto asset = std::make_unique<MediaAsset>(idStream.str(), path, kind);
    MediaAsset* raw = asset.get();
    mediaBin_[raw->id()] = std::move(asset);
    return raw;
}

MediaAsset* Project::findMedia(const std::string& id) const {
    auto it = mediaBin_.find(id);
    return it == mediaBin_.end() ? nullptr : it->second.get();
}

Timeline* Project::createTimeline(const std::string& name, int fpsNum, int fpsDen,
                                     int width, int height) {
    std::ostringstream idStream;
    idStream << "timeline_" << nextTimelineId_++;
    auto tl = std::make_unique<Timeline>(idStream.str(), name, fpsNum, fpsDen, width, height);
    Timeline* raw = tl.get();
    timelines_[raw->id()] = std::move(tl);
    return raw;
}

Timeline* Project::findTimeline(const std::string& id) const {
    auto it = timelines_.find(id);
    return it == timelines_.end() ? nullptr : it->second.get();
}

} // namespace ds::core
