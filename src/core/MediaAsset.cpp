#include "core/MediaAsset.h"

namespace ds::core {

MediaAsset::MediaAsset(std::string id, std::string sourcePath, MediaKind kind)
    : id_(std::move(id)), sourcePath_(std::move(sourcePath)), kind_(kind) {}

} // namespace ds::core
