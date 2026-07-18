#pragma once
#include <QWidget>
#include <QOpenGLWidget>

namespace ds::ui {

// The actual frame surface. Renders composited frames handed to it by
// PlaybackController via a texture upload each tick; kept as a thin
// QOpenGLWidget subclass so the heavy lifting stays in engine/RenderGraph.
class ViewerSurface : public QOpenGLWidget {
    Q_OBJECT
public:
    explicit ViewerSurface(QWidget* parent = nullptr);
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
};

// Viewer panel: surface + transport bar (play/pause/scrub) + timecode +
// resolution/quality indicator (shows current dynamic playback scale).
class ViewerPanel : public QWidget {
    Q_OBJECT
public:
    explicit ViewerPanel(QWidget* parent = nullptr);

private:
    ViewerSurface* surface_ = nullptr;
};

} // namespace ds::ui
