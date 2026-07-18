#include "ui/MediaBinPanel.h"
#include <QVBoxLayout>
#include <QLineEdit>

namespace ds::ui {

MediaBinPanel::MediaBinPanel(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);

    auto* search = new QLineEdit();
    search->setPlaceholderText("Search media... (AI Media Tagging aware)");
    layout->addWidget(search);

    list_ = new QListWidget(this);
    list_->setIconSize(QSize(120, 68));
    list_->setViewMode(QListView::IconMode);
    list_->setResizeMode(QListView::Adjust);
    list_->setDragEnabled(true); // drag onto TimelinePanel
    layout->addWidget(list_, 1);

    // TODO: connect search text changes to a filter over asset names/tags,
    // where tags are populated asynchronously by the AI Media Tagging
    // feature once implemented.
}

void MediaBinPanel::addAsset(ds::core::MediaAsset* asset) {
    auto* item = new QListWidgetItem(QString::fromStdString(asset->sourcePath()));
    item->setData(Qt::UserRole, QString::fromStdString(asset->id()));
    list_->addItem(item);
    // TODO: kick off thumbnail extraction (first frame or filmstrip) on a
    // worker thread and set item->setIcon(...) when ready.
}

void MediaBinPanel::updateProxyProgress(const std::string& assetId, float pct) {
    (void)assetId; (void)pct;
    // TODO: find matching QListWidgetItem by UserRole id and update a
    // small progress overlay/badge on its icon.
}

} // namespace ds::ui
