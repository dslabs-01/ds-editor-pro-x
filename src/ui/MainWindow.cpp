#include "ui/MainWindow.h"
#include "ui/TimelinePanel.h"
#include "ui/ViewerPanel.h"
#include "ui/MediaBinPanel.h"
#include "ui/InspectorPanel.h"

#include <QMenuBar>
#include <QToolBar>
#include <QDockWidget>
#include <QFileDialog>
#include <QApplication>
#include <QPalette>

namespace ds::ui {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("DS Editor Pro X");
    applyDarkTheme();

    project_ = std::make_unique<ds::core::Project>("Untitled Project");
    project_->createTimeline("Timeline 1", 30, 1, 1920, 1080);

    buildMenus();
    buildToolbar();
    buildDockLayout();
}

void MainWindow::applyDarkTheme() {
    // Modern dark theme baseline; a full theme system (incl. light theme +
    // theme API for plugin marketplace) lives in a future ui/ThemeManager.
    qApp->setStyle("Fusion");
    QPalette p;
    p.setColor(QPalette::Window, QColor(24, 24, 27));
    p.setColor(QPalette::WindowText, QColor(228, 228, 231));
    p.setColor(QPalette::Base, QColor(15, 15, 17));
    p.setColor(QPalette::AlternateBase, QColor(30, 30, 33));
    p.setColor(QPalette::Text, QColor(228, 228, 231));
    p.setColor(QPalette::Button, QColor(38, 38, 42));
    p.setColor(QPalette::ButtonText, QColor(228, 228, 231));
    p.setColor(QPalette::Highlight, QColor(59, 130, 246)); // brand accent blue
    p.setColor(QPalette::HighlightedText, Qt::white);
    qApp->setPalette(p);
}

void MainWindow::buildMenus() {
    auto* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("New Project", this, &MainWindow::onNewProject, QKeySequence::New);
    fileMenu->addAction("Open Project...", this, &MainWindow::onOpenProject, QKeySequence::Open);
    fileMenu->addAction("Save Project", this, &MainWindow::onSaveProject, QKeySequence::Save);
    fileMenu->addSeparator();
    fileMenu->addAction("Import Media...", this, &MainWindow::onImportMedia, QKeySequence("Ctrl+I"));
    fileMenu->addSeparator();
    fileMenu->addAction("Export...", this, &MainWindow::onExport, QKeySequence("Ctrl+E"));

    menuBar()->addMenu("&Edit");
    menuBar()->addMenu("&Timeline");
    menuBar()->addMenu("&Color");
    menuBar()->addMenu("&Effects");
    menuBar()->addMenu("&View");
    auto* windowMenu = menuBar()->addMenu("&Window");
    windowMenu->addAction("Reset Workspace"); // TODO: wire to saved dock geometry
    menuBar()->addMenu("&Help");
}

void MainWindow::buildToolbar() {
    auto* toolbar = addToolBar("Main");
    toolbar->setMovable(false);
    toolbar->addAction("Cut");     // Blade tool
    toolbar->addAction("Ripple");
    toolbar->addAction("Roll");
    toolbar->addAction("Slip");
    toolbar->addAction("Slide");
    toolbar->addSeparator();
    toolbar->addAction("Magnetic Timeline"); // toggles Track::setMagnetic on all tracks
}

void MainWindow::buildDockLayout() {
    setDockNestingEnabled(true);

    viewerPanel_ = new ViewerPanel(this);
    auto* viewerDock = new QDockWidget("Viewer", this);
    viewerDock->setWidget(viewerPanel_);
    addDockWidget(Qt::TopDockWidgetArea, viewerDock);

    mediaBinPanel_ = new MediaBinPanel(this);
    auto* mediaBinDock = new QDockWidget("Media Bin", this);
    mediaBinDock->setWidget(mediaBinPanel_);
    addDockWidget(Qt::LeftDockWidgetArea, mediaBinDock);

    inspectorPanel_ = new InspectorPanel(this);
    auto* inspectorDock = new QDockWidget("Inspector", this);
    inspectorDock->setWidget(inspectorPanel_);
    addDockWidget(Qt::RightDockWidgetArea, inspectorDock);

    timelinePanel_ = new TimelinePanel(this);
    auto* timelineDock = new QDockWidget("Timeline", this);
    timelineDock->setWidget(timelinePanel_);
    addDockWidget(Qt::BottomDockWidgetArea, timelineDock);

    auto* activeTimeline = project_->timelines().begin()->second.get();
    timelinePanel_->setTimeline(activeTimeline);
}

void MainWindow::onNewProject() {
    project_ = std::make_unique<ds::core::Project>("Untitled Project");
    project_->createTimeline("Timeline 1", 30, 1, 1920, 1080);
    timelinePanel_->setTimeline(project_->timelines().begin()->second.get());
}

void MainWindow::onOpenProject() {
    QString path = QFileDialog::getOpenFileName(this, "Open Project", QString(), "DS Project (*.dsproj)");
    if (path.isEmpty()) return;
    // TODO: deserialize Project from JSON/binary at `path`.
}

void MainWindow::onSaveProject() {
    QString path = QFileDialog::getSaveFileName(this, "Save Project", QString(), "DS Project (*.dsproj)");
    if (path.isEmpty()) return;
    // TODO: serialize project_ to `path`, encrypted per "Encrypted project files".
}

void MainWindow::onImportMedia() {
    QStringList paths = QFileDialog::getOpenFileNames(this, "Import Media");
    for (const auto& path : paths) {
        auto* asset = project_->importMedia(path.toStdString(), ds::core::MediaKind::VideoFile);
        mediaBinPanel_->addAsset(asset);
        // TODO: kick MediaProbe + ProxyManager::requestProxy on a worker thread.
    }
}

void MainWindow::onExport() {
    // TODO: open ExportDialog (format/codec/preset picker backed by
    // EncodeProfile presets), then ExportQueue::enqueue the active timeline.
}

} // namespace ds::ui
