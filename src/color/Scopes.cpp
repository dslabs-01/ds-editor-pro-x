#include "color/Scopes.h"

namespace ds::color {

ScopeData ScopeAnalyzer::compute(ScopeType type, const uint8_t* rgbaBuffer, int width, int height) {
    ScopeData data;
    data.type = type;
    data.width = width;
    data.height = height;
    // TODO: Waveform -> per-column luma histogram; Vectorscope -> Cb/Cr
    // scatter mapped to polar hue/saturation; Histogram -> per-channel
    // 256-bucket counts; RGBParade -> three side-by-side waveforms.
    // GPU compute shader recommended for full-res frames to stay real-time.
    (void)rgbaBuffer;
    return data;
}

} // namespace ds::color
