#pragma once
#include <string>
#include <optional>
#include <cstdint>

namespace ds::core {

enum class MediaKind { VideoFile, AudioFile, ImageFile, TimelineReference, GeneratedTitle };

// Technical metadata gathered during import/indexing (used by AI Media
// Tagging, proxy generation decisions, and the media bin UI).
struct MediaProbe {
    int width = 0;
    int height = 0;
    double frameRate = 0.0;
    int64_t durationFrames = 0;
    std::string codec;
    std::string colorSpace;
    bool hasAlpha = false;
    int audioChannels = 0;
    int audioSampleRate = 0;
};

class MediaAsset {
public:
    MediaAsset(std::string id, std::string sourcePath, MediaKind kind);

    const std::string& id() const { return id_; }
    const std::string& sourcePath() const { return sourcePath_; }
    MediaKind kind() const { return kind_; }

    void setProbe(MediaProbe p) { probe_ = std::move(p); }
    const std::optional<MediaProbe>& probe() const { return probe_; }

    // If proxy generation ran, this points at a lower-res transcode used
    // for smooth playback on low-end hardware; original is used for export.
    void setProxyPath(std::string path) { proxyPath_ = std::move(path); }
    const std::optional<std::string>& proxyPath() const { return proxyPath_; }

private:
    std::string id_;
    std::string sourcePath_;
    MediaKind kind_;
    std::optional<MediaProbe> probe_;
    std::optional<std::string> proxyPath_;
};

} // namespace ds::core
