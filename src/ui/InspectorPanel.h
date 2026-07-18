#pragma once
#include <QWidget>
#include <QTabWidget>
#include "core/Clip.h"

namespace ds::ui {

// Context-sensitive inspector: shows transform/effects/audio/color tabs for
// whatever is currently selected (clip, adjustment layer, track, or the
// timeline itself for master grade). Effect params drive core::EffectInstance
// values with keyframe editing surfaced here in a future KeyframeEditor.
class InspectorPanel : public QWidget {
    Q_OBJECT
public:
    explicit InspectorPanel(QWidget* parent = nullptr);

    void showClip(ds::core::Clip* clip);
    void clear();

private:
    QTabWidget* tabs_ = nullptr;
    ds::core::Clip* currentClip_ = nullptr;
};

} // namespace ds::ui
