#include <QApplication>
#include <QSurfaceFormat>
#include "ui/MainWindow.h"

int main(int argc, char* argv[]) {
    // Request a GL profile suitable for the viewer's real-time compositing;
    // falls back gracefully via Qt if unavailable on the host.
    QSurfaceFormat fmt;
    fmt.setSwapInterval(0); // we manage our own frame pacing to hit target FPS
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication app(argc, argv);
    QApplication::setApplicationName("DS Editor Pro X");
    QApplication::setOrganizationName("DS Labs");

    ds::ui::MainWindow window;
    window.resize(1600, 950);
    window.show();

    return app.exec();
}
