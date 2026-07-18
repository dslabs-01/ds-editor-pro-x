#pragma once
#include <cstdint>
#include <vector>

namespace ds::color {

enum class ScopeType { Waveform, Vectorscope, Histogram, RGBParade };

// Computed once per displayed frame (not per playback frame) so scopes stay
// responsive without taxing the render loop during scrubbing.
struct ScopeData {
    ScopeType type;
    int width = 0, height = 0;
    std::vector<float> buckets; // interpretation depends on `type`
};

// Analyzes a composited RGBA frame into scope visualizations for the color
// panel. Pure/stateless so it can run on a worker thread off the UI thread.
class ScopeAnalyzer {
public:
    static ScopeData compute(ScopeType type, const uint8_t* rgbaBuffer, int width, int height);
};

} // namespace ds::color
