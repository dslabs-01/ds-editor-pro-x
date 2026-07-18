#include "ui/ViewerPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>

namespace ds::ui {

ViewerSurface::ViewerSurface(QWidget* parent) : QOpenGLWidget(parent) {}

void ViewerSurface::initializeGL() {
    // TODO: compile the compositor's shader programs (color pipeline,
    // transitions, text/title rendering) once here; reused every paintGL.
}

void ViewerSurface::paintGL() {
    // TODO: upload the latest DecodedFrame (from PlaybackController's
    // frame-ready callback) as a texture and draw a fullscreen quad.
}

void ViewerSurface::resizeGL(int w, int h) {
    (void)w; (void)h;
}

ViewerPanel::ViewerPanel(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);

    surface_ = new ViewerSurface(this);
    surface_->setMinimumHeight(360);
    layout->addWidget(surface_, 1);

    auto* transport = new QHBoxLayout();
    auto* timecode = new QLabel("00:00:00:00");
    timecode->setStyleSheet("font-family: monospace; font-size: 13px;");
    auto* playBtn = new QPushButton("▶");
    playBtn->setFixedWidth(40);
    auto* scrubber = new QSlider(Qt::Horizontal);

    transport->addWidget(timecode);
    transport->addWidget(playBtn);
    transport->addWidget(scrubber, 1);
    layout->addLayout(transport);

    // TODO: connect playBtn -> PlaybackController::play/pause,
    // scrubber -> PlaybackController::seek, and a frame-ready callback that
    // triggers surface_->update() with the new texture.
}

} // namespace ds::ui
