// Standalone proof harness — NOT part of the app build, just verifies
// DecodeEngine actually decodes real frames from a real file.
#include "engine/DecodeEngine.h"
#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: decode_probe <video path> [frame_number]\n";
        return 1;
    }
    std::string path = argv[1];
    ds::core::FrameTime targetFrame = argc > 2 ? std::stoll(argv[2]) : 10;

    ds::engine::DecodeEngine engine(path);
    if (!engine.open()) {
        std::cerr << "OPEN FAILED: " << engine.lastError() << "\n";
        return 1;
    }

    std::cout << "Opened: " << path << "\n"
              << "  resolution: " << engine.width() << "x" << engine.height() << "\n"
              << "  frame rate: " << engine.frameRate() << "\n"
              << "  total frames (est): " << engine.totalFrames() << "\n";

    ds::engine::DecodedFrame frame;
    bool ok = engine.decodeFrame(targetFrame, frame);
    if (!ok) {
        std::cerr << "DECODE FAILED at frame " << targetFrame << ": " << engine.lastError() << "\n";
        return 1;
    }

    std::cout << "Decoded frame " << targetFrame
              << " -> " << frame.width << "x" << frame.height
              << " RGBA, " << frame.data.size() << " bytes, pts=" << frame.ptsSeconds << "s\n";

    // Dump as a raw PPM (P6) so it's trivially viewable — proves the pixel
    // data is real image content, not zeroed/garbage memory.
    std::string outPath = "/tmp/decoded_frame.ppm";
    std::ofstream out(outPath, std::ios::binary);
    out << "P6\n" << frame.width << " " << frame.height << "\n255\n";
    for (size_t i = 0; i < frame.data.size(); i += 4) {
        out.put(frame.data[i]);     // R
        out.put(frame.data[i + 1]); // G
        out.put(frame.data[i + 2]); // B
    }
    out.close();
    std::cout << "Wrote " << outPath << "\n";

    // Quick sanity check: is the frame not just solid black? testsrc
    // produces a colorful moving pattern, so a real decode should have
    // varied pixel values.
    size_t nonZero = 0;
    for (size_t i = 0; i < frame.data.size(); i += 4) {
        if (frame.data[i] != 0 || frame.data[i+1] != 0 || frame.data[i+2] != 0) nonZero++;
    }
    std::cout << "Non-black pixels: " << nonZero << " / " << (frame.data.size() / 4) << "\n";

    engine.close();
    return 0;
}
