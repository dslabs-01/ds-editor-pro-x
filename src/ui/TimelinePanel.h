#pragma once
#include <QWidget>
#include "core/Timeline.h"

namespace ds::ui {

// Custom-painted (not item-view-based) multi-track timeline for performance
// at high clip counts. Handles blade/ripple/roll/slip tools, drag-drop from
// the media bin, marker rendering, and zoom. Kept as a single QWidget with
// manual paintEvent so we control exactly how many clips get redrawn per
// frame during scrubbing (critical for the "extremely fast" requirement).
class TimelinePanel : public QWidget {
    Q_OBJECT
public:
    explicit TimelinePanel(QWidget* parent = nullptr);

    void setTimeline(ds::core::Timeline* timeline);

    enum class Tool { Select, Blade, Ripple, Roll, Slip, Slide };
    void setActiveTool(Tool t) { activeTool_ = t; update(); }

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override; // zoom

private:
    int frameToPixel(ds::core::FrameTime frame) const;
    ds::core::FrameTime pixelToFrame(int px) const;

    ds::core::Timeline* timeline_ = nullptr; // non-owning
    Tool activeTool_ = Tool::Select;
    double pixelsPerFrame_ = 1.0;
    int horizontalScrollFrames_ = 0;
    ds::core::Clip* draggingClip_ = nullptr;
};

} // namespace ds::ui
