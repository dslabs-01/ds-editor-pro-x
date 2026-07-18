#pragma once
#include "core/Clip.h"
#include <vector>
#include <memory>
#include <optional>

namespace ds::core {

enum class TrackType { Video, Audio, Subtitle };

// A Track owns an ordered set of non-overlapping clips (unless overlap is
// explicitly allowed for transition regions). Unlimited tracks are supported
// by the data model — UI/perf limits, not architectural ones, govern how many
// are practical on a given machine.
class Track {
public:
    Track(std::string id, TrackType type, int index);

    const std::string& id() const { return id_; }
    TrackType type() const { return type_; }
    int index() const { return index_; }

    bool muted() const { return muted_; }
    void setMuted(bool v) { muted_ = v; }
    bool locked() const { return locked_; }
    void setLocked(bool v) { locked_ = v; }
    bool solo() const { return solo_; }
    void setSolo(bool v) { solo_ = v; }

    // Ownership stays with the track; UI/engine hold non-owning references.
    Clip* addClip(std::unique_ptr<Clip> clip);
    bool removeClip(const std::string& clipId);
    Clip* clipAt(FrameTime frame) const; // returns clip under playhead, or nullptr

    const std::vector<std::unique_ptr<Clip>>& clips() const { return clips_; }

    // Magnetic timeline: when enabled, removing/moving a clip closes gaps
    // automatically on this track.
    bool magnetic() const { return magnetic_; }
    void setMagnetic(bool v) { magnetic_ = v; }
    void closeGaps(); // ripple-close empty space between clips

private:
    std::string id_;
    TrackType type_;
    int index_;
    bool muted_ = false;
    bool locked_ = false;
    bool solo_ = false;
    bool magnetic_ = false;
    std::vector<std::unique_ptr<Clip>> clips_;
};

} // namespace ds::core
