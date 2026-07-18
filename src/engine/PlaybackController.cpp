#include "engine/PlaybackController.h"

namespace ds::engine {

PlaybackController::PlaybackController(ds::core::Timeline* timeline, RenderGraph::MediaPathResolver resolver)
    : timeline_(timeline), renderGraph_(std::make_unique<RenderGraph>(timeline, std::move(resolver))) {}

void PlaybackController::play() { state_ = PlaybackState::Playing; }
void PlaybackController::pause() { state_ = PlaybackState::Stopped; }
void PlaybackController::stop() {
    state_ = PlaybackState::Stopped;
    timeline_->setPlayhead(0);
}

void PlaybackController::seek(ds::core::FrameTime frame) {
    timeline_->setPlayhead(frame);
    if (state_ != PlaybackState::Playing) {
        auto frameData = renderGraph_->renderFrame(frame, 1.0);
        if (frameReady_) frameReady_(frameData);
    }
}

double PlaybackController::chooseResolutionScale() const {
    // TODO: read live CPU/GPU load + available RAM (see AI Performance
    // Monitor) and drop resolution under load; return to 1.0 when idle
    // headroom allows, per "Dynamic playback resolution".
    return 1.0;
}

void PlaybackController::tick() {
    if (state_ != PlaybackState::Playing) return;
    ds::core::FrameTime next = timeline_->playhead() + 1;
    if (next >= timeline_->durationFrames()) {
        stop();
        return;
    }
    timeline_->setPlayhead(next);
    double scale = chooseResolutionScale();
    auto frameData = renderGraph_->renderFrame(next, scale);
    if (frameReady_) frameReady_(frameData);
    // TODO: schedule next tick() via a Qt QTimer / high-res timer at
    // 1000.0 / timeline_->frameRate() ms, owned by the UI layer.
}

} // namespace ds::engine
