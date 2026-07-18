#pragma once
#include <string>
#include <vector>
#include <array>

namespace ds::color {

// A single point on a curves control (RGB master or per-channel).
struct CurvePoint { float x; float y; };

// One node in a node-based grade. Nodes chain serially by default; the
// UI can also branch/merge nodes (layer nodes), tracked by parentIds.
struct GradeNode {
    std::string id;
    std::string type; // "primary" | "curves" | "hsl_qualifier" | "power_window" | "lut"
    std::vector<std::string> parentIds;

    // Primary wheel adjustments (lift/gamma/gain), each RGB.
    std::array<float, 3> lift{0, 0, 0};
    std::array<float, 3> gamma{1, 1, 1};
    std::array<float, 3> gain{1, 1, 1};

    std::vector<CurvePoint> curveMaster;
    std::vector<CurvePoint> curveRed, curveGreen, curveBlue;

    std::string lutPath; // for type == "lut"
    bool enabled = true;
};

// Per-clip (or per-adjustment-layer) grade: an ordered graph of GradeNodes
// evaluated through OCIO for color-space-correct math, matching the
// "Node-based grading" + "ACES workflow" + "LUT support" requirements.
class ColorPipeline {
public:
    ColorPipeline();

    GradeNode& addNode(const std::string& type);
    bool removeNode(const std::string& nodeId);
    std::vector<GradeNode>& nodes() { return nodes_; }

    void setWorkingColorSpace(const std::string& csName) { workingSpace_ = csName; }
    void setDisplayTransform(const std::string& viewTransform) { displayTransform_ = viewTransform; }

    // Applies the full node graph to a frame buffer in-place. Real
    // implementation dispatches to GPU shaders generated from the OCIO
    // processor for the current working/display transform pair.
    void applyToFrame(uint8_t* rgbaBuffer, int width, int height) const;

private:
    std::vector<GradeNode> nodes_;
    std::string workingSpace_ = "ACEScg";
    std::string displayTransform_ = "Rec.709";
    int nextNodeId_ = 0;
};

} // namespace ds::color
