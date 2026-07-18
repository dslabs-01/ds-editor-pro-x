// Proof harness for seek correctness + performance: decodes several
// out-of-order frames from a single open() call and times each, to show
// backward/far-forward seeks don't degrade into O(n) scans from frame 0.
#include "engine/DecodeEngine.h"
#include <iostream>
#include <chrono>
#include <vector>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: seek_probe <video path>\n";
        return 1;
    }
    ds::engine::DecodeEngine engine(argv[1]);
    if (!engine.open()) {
        std::cerr << "OPEN FAILED: " << engine.lastError() << "\n";
        return 1;
    }
    std::cout << "Opened " << argv[1] << " | " << engine.width() << "x" << engine.height()
              << " @ " << engine.frameRate() << "fps, ~" << engine.totalFrames() << " frames\n\n";

    // Deliberately out of order: forward, big forward jump, backward jump,
    // small backward jump, forward again.
    std::vector<ds::core::FrameTime> requests = {100, 1400, 50, 900, 920, 10, 1499};

    for (auto target : requests) {
        ds::engine::DecodedFrame frame;
        auto t0 = std::chrono::high_resolution_clock::now();
        bool ok = engine.decodeFrame(target, frame);
        auto t1 = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

        if (!ok && frame.data.empty()) {
            std::cout << "frame " << target << " -> FAILED: " << engine.lastError() << "\n";
            continue;
        }
        std::cout << "requested frame " << target
                   << " -> got frame " << frame.frameNumber
                   << " (pts=" << frame.ptsSeconds << "s)"
                   << " in " << ms << " ms\n";
    }

    engine.close();
    return 0;
}
