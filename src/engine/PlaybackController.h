#pragma once
#include "core/Timeline.h"
#include "engine/RenderGraph.h"
#include <memory>
#include <atomic>
#include <functional>

namespace ds::engine {

enum class PlaybackState { Stopped, Playing, Scrubbing };

// Owns the playback clock and decides, frame by frame, what resolution to
// render at so the app hits its target frame rate ("Dynamic playback
// resolution" + "Intelligent RAM management" live here). On a 4GB machine
// this aggressively drops to quarter-res proxy frames during playback and
// only renders full-res on pause, matching the low-end hardware target.
class PlaybackController {
public:
    PlaybackController(ds::core::Timeline* timeline, RenderGraph::MediaPathResolver resolver);

    void play();
    void pause();
    void stop();
    void seek(ds::core::FrameTime frame);

    PlaybackState state() const { return state_; }

    // Called by the UI's frame-ready signal each time a new composited
    // frame is available to present.
    using FrameReadyCallback = std::function<void(const DecodedFrame&)>;
    void setFrameReadyCallback(FrameReadyCallback cb) { frameReady_ = std::move(cb); }

private:
    void tick(); // scheduled on a timer at timeline_->frameRate()
    double chooseResolutionScale() const; // consults live perf metrics

    ds::core::Timeline* timeline_;
    std::unique_ptr<RenderGraph> renderGraph_;
    std::atomic<PlaybackState> state_{PlaybackState::Stopped};
    FrameReadyCallback frameReady_;
};

} // namespace ds::engine
