#pragma once
#include <QWidget>
#include <QListWidget>
#include "core/MediaAsset.h"

namespace ds::ui {

// Media bin: thumbnail grid/list of imported assets, drag source for the
// timeline. Proxy/indexing status badges surface ProxyManager progress.
class MediaBinPanel : public QWidget {
    Q_OBJECT
public:
    explicit MediaBinPanel(QWidget* parent = nullptr);

    void addAsset(ds::core::MediaAsset* asset);
    void updateProxyProgress(const std::string& assetId, float pct);

private:
    QListWidget* list_ = nullptr;
};

} // namespace ds::ui
