#include "ui/TimelinePanel.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>

namespace ds::ui {

TimelinePanel::TimelinePanel(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(220);
    setMouseTracking(true);
}

void TimelinePanel::setTimeline(ds::core::Timeline* timeline) {
    timeline_ = timeline;
    update();
}

int TimelinePanel::frameToPixel(ds::core::FrameTime frame) const {
    return static_cast<int>((frame - horizontalScrollFrames_) * pixelsPerFrame_);
}

ds::core::FrameTime TimelinePanel::pixelToFrame(int px) const {
    return static_cast<ds::core::FrameTime>(px / pixelsPerFrame_) + horizontalScrollFrames_;
}

void TimelinePanel::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor(20, 20, 23));

    if (!timeline_) return;

    const int trackHeight = 56;
    const int trackGap = 2;
    int y = 4;

    for (const auto& track : timeline_->tracks()) {
        QRect trackRect(0, y, width(), trackHeight);
        painter.fillRect(trackRect, QColor(30, 30, 34));

        for (const auto& clip : track->clips()) {
            int x1 = frameToPixel(clip->timelineStart());
            int x2 = frameToPixel(clip->timelineEnd());
            if (x2 < 0 || x1 > width()) continue; // off-screen culling

            QRect clipRect(x1, y + 2, std::max(2, x2 - x1), trackHeight - 4);
            QColor clipColor = track->type() == ds::core::TrackType::Video
                ? QColor(59, 130, 246, 200)
                : QColor(34, 197, 94, 200);
            painter.setBrush(clipColor);
            painter.setPen(clip->locked() ? QColor(250, 204, 21) : QColor(0, 0, 0, 0));
            painter.drawRoundedRect(clipRect, 4, 4);
        }
        y += trackHeight + trackGap;
    }

    // Playhead
    int playheadX = frameToPixel(timeline_->playhead());
    painter.setPen(QPen(QColor(239, 68, 68), 2));
    painter.drawLine(playheadX, 0, playheadX, height());

    // Markers
    painter.setPen(Qt::NoPen);
    for (const auto& marker : timeline_->markers()) {
        int mx = frameToPixel(marker.frame);
        painter.setBrush(QColor(marker.colorHex));
        painter.drawEllipse(QPoint(mx, 6), 4, 4);
    }
}

void TimelinePanel::mousePressEvent(QMouseEvent* event) {
    if (!timeline_) return;
    ds::core::FrameTime frame = pixelToFrame(event->pos().x());

    switch (activeTool_) {
        case Tool::Blade:
            // TODO: find clip under (frame, track-at-y), split into two
            // clips at `frame` by cloning with adjusted trim handles.
            break;
        case Tool::Select:
        default:
            // TODO: hit-test clip under cursor, begin drag for move/ripple.
            timeline_->setPlayhead(frame);
            update();
            break;
    }
}

void TimelinePanel::mouseMoveEvent(QMouseEvent* event) {
    (void)event;
    // TODO: if draggingClip_, update its position per activeTool_ semantics
    // (ripple pushes neighbors, roll adjusts adjacent edge, slip/slide per
    // their defined behavior in core::Clip).
}

void TimelinePanel::mouseReleaseEvent(QMouseEvent* event) {
    (void)event;
    draggingClip_ = nullptr;
}

void TimelinePanel::wheelEvent(QWheelEvent* event) {
    double factor = event->angleDelta().y() > 0 ? 1.15 : 1.0 / 1.15;
    pixelsPerFrame_ = std::clamp(pixelsPerFrame_ * factor, 0.02, 20.0);
    update();
}

} // namespace ds::ui
