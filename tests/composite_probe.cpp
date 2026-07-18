// Proof harness: builds a real Timeline with two video tracks, each
// referencing a real media file, and renders frames through RenderGraph
// to prove the full data path — Timeline -> Track -> Clip -> DecodeEngine
// -> composited pixels — actually works, not just each piece in isolation.
#include "core/Timeline.h"
#include "engine/RenderGraph.h"
#include <iostream>
#include <fstream>
#include <unordered_map>

static void writePPM(const std::string& path, const ds::engine::DecodedFrame& f) {
    std::ofstream out(path, std::ios::binary);
    out << "P6\n" << f.width << " " << f.height << "\n255\n";
    for (size_t i = 0; i + 3 < f.data.size(); i += 4) {
        out.put(f.data[i]); out.put(f.data[i+1]); out.put(f.data[i+2]);
    }
}

int main() {
    using namespace ds::core;

    Timeline timeline("tl", "Composite Test", 25, 1, 640, 360);

    // Bottom track: colorbars (testsrc), full 75-frame duration.
    Track* bottom = timeline.addTrack(TrackType::Video);
    auto clipA = std::make_unique<Clip>("clipA", ClipType::Video, "media_bottom");
    clipA->setTrim({.inPoint = 0, .outPoint = 74});
    clipA->moveTo(0);
    bottom->addClip(std::move(clipA));

    // Top track: smptebars overlay, only frames 30-50 (covers the middle).
    Track* top = timeline.addTrack(TrackType::Video);
    auto clipB = std::make_unique<Clip>("clipB", ClipType::Video, "media_top");
    clipB->setTrim({.inPoint = 0, .outPoint = 19}); // 20 frames of source
    clipB->moveTo(30); // starts at timeline frame 30
    top->addClip(std::move(clipB));

    // Resolver: maps our fake mediaAssetIds to real files on disk.
    std::unordered_map<std::string, std::string> paths = {
        {"media_bottom", "/tmp/testmedia/sample.mp4"},
        {"media_top", "/tmp/testmedia/overlay.mp4"},
    };
    ds::engine::RenderGraph graph(&timeline, [&](const std::string& id) {
        auto it = paths.find(id);
        return it != paths.end() ? it->second : std::string();
    });

    // Frame 10: only bottom track active -> should look like colorbars (testsrc).
    // Frame 40: both tracks active, top should win -> should look like smptebars.
    // Frame 65: bottom track only again (top clip ended at frame 50) -> colorbars.
    struct Case { ds::core::FrameTime frame; std::string outPath; std::string expect; };
    std::vector<Case> cases = {
        {10, "/tmp/composite_frame10_bottom_only.ppm", "bottom track only (testsrc pattern)"},
        {40, "/tmp/composite_frame40_top_wins.ppm", "top track composited over bottom (smptebars pattern)"},
        {65, "/tmp/composite_frame65_bottom_again.ppm", "top clip ended, bottom track only again"},
    };

    for (auto& c : cases) {
        auto frame = graph.renderFrame(c.frame, 1.0);
        if (frame.data.empty()) {
            std::cerr << "frame " << c.frame << " FAILED: " << graph.lastError() << "\n";
            continue;
        }
        writePPM(c.outPath, frame);
        size_t nonBlack = 0;
        for (size_t i = 0; i + 3 < frame.data.size(); i += 4)
            if (frame.data[i] || frame.data[i+1] || frame.data[i+2]) nonBlack++;
        std::cout << "frame " << c.frame << " (" << c.expect << ") -> "
                  << frame.width << "x" << frame.height
                  << ", non-black px: " << nonBlack << "/" << frame.data.size()/4
                  << " -> " << c.outPath << "\n";
    }
    return 0;
}
