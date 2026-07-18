#pragma once
#include <string>

namespace ds::exportmod {

enum class Container { MP4, MOV, AVI, MKV, MXF, WebM, GIF, ImageSequence };
enum class VideoCodec { H264, H265, AV1, ProRes, DNxHR, PNGSeq, JPEGSeq, EXRSeq };
enum class AudioCodec { AAC, PCM, ALAC, Opus, None };

struct EncodeProfile {
    Container container = Container::MP4;
    VideoCodec videoCodec = VideoCodec::H264;
    AudioCodec audioCodec = AudioCodec::AAC;

    int width = 1920;
    int height = 1080;
    double frameRate = 30.0;
    int videoBitrateKbps = 20000; // ignored for lossless/ProRes-style codecs
    bool hdr = false;
    bool useGpuEncode = true; // NVENC/QuickSync/AMF when available

    // Convenience presets matching common delivery targets; UI exposes
    // these as one-click options, with full manual override available.
    static EncodeProfile youtube4K();
    static EncodeProfile youtube1080p();
    static EncodeProfile proResMaster();
    static EncodeProfile socialVertical(); // 1080x1920 for Reels/Shorts/TikTok
};

} // namespace ds::exportmod
