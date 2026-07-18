#include "color/ColorPipeline.h"
#include <algorithm>
#include <sstream>

namespace ds::color {

ColorPipeline::ColorPipeline() = default;

GradeNode& ColorPipeline::addNode(const std::string& type) {
    GradeNode node;
    std::ostringstream idStream;
    idStream << "node_" << nextNodeId_++;
    node.id = idStream.str();
    node.type = type;
    nodes_.push_back(std::move(node));
    return nodes_.back();
}

bool ColorPipeline::removeNode(const std::string& nodeId) {
    auto it = std::find_if(nodes_.begin(), nodes_.end(),
        [&](const GradeNode& n) { return n.id == nodeId; });
    if (it == nodes_.end()) return false;
    nodes_.erase(it);
    return true;
}

void ColorPipeline::applyToFrame(uint8_t* rgbaBuffer, int width, int height) const {
    // TODO: for each enabled node in dependency order, build/cache an OCIO
    // GpuShaderDesc for (workingSpace_ -> node transform -> displayTransform_)
    // and run it as a fragment shader pass over rgbaBuffer. CPU path (for
    // headless export without a GPU context) uses OCIO's CPU processor
    // instead, applied per-scanline.
    (void)rgbaBuffer; (void)width; (void)height;
}

} // namespace ds::color
