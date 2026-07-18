#pragma once
#include <QMainWindow>
#include <memory>
#include "core/Project.h"

namespace ds::ui {

class TimelinePanel;
class ViewerPanel;
class MediaBinPanel;
class InspectorPanel;

// Top-level window: Resolve/Premiere-style dockable panel layout built on
// QDockWidget so users can rearrange, float, or save workspace presets
// (matching "Fully customizable workspace" + "Workspace presets").
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);

private slots:
    void onNewProject();
    void onOpenProject();
    void onSaveProject();
    void onImportMedia();
    void onExport();

private:
    void buildMenus();
    void buildDockLayout();
    void buildToolbar();
    void applyDarkTheme();

    std::unique_ptr<ds::core::Project> project_;

    ViewerPanel* viewerPanel_ = nullptr;
    TimelinePanel* timelinePanel_ = nullptr;
    MediaBinPanel* mediaBinPanel_ = nullptr;
    InspectorPanel* inspectorPanel_ = nullptr;
};

} // namespace ds::ui
