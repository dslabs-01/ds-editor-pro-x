#include "ui/InspectorPanel.h"
#include <QVBoxLayout>
#include <QLabel>

namespace ds::ui {

InspectorPanel::InspectorPanel(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);

    tabs_ = new QTabWidget(this);
    tabs_->addTab(new QWidget(), "Transform");
    tabs_->addTab(new QWidget(), "Effects");
    tabs_->addTab(new QWidget(), "Color");
    tabs_->addTab(new QWidget(), "Audio");
    layout->addWidget(tabs_);

    // TODO: each tab's widget is built dynamically from the selected
    // clip's EffectInstance list / ColorPipeline node graph, with controls
    // that write back into core::Clip through UndoStack Commands so every
    // tweak is undoable.
}

void InspectorPanel::showClip(ds::core::Clip* clip) {
    currentClip_ = clip;
    // TODO: rebuild tab contents from clip->effects().
}

void InspectorPanel::clear() {
    currentClip_ = nullptr;
}

} // namespace ds::ui
