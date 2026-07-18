#include "export/EncodeProfile.h"

namespace ds::exportmod {

EncodeProfile EncodeProfile::youtube4K() {
    EncodeProfile p;
    p.container = Container::MP4;
    p.videoCodec = VideoCodec::H265;
    p.width = 3840; p.height = 2160;
    p.videoBitrateKbps = 45000;
    return p;
}

EncodeProfile EncodeProfile::youtube1080p() {
    EncodeProfile p;
    p.container = Container::MP4;
    p.videoCodec = VideoCodec::H264;
    p.width = 1920; p.height = 1080;
    p.videoBitrateKbps = 12000;
    return p;
}

EncodeProfile EncodeProfile::proResMaster() {
    EncodeProfile p;
    p.container = Container::MOV;
    p.videoCodec = VideoCodec::ProRes;
    p.audioCodec = AudioCodec::PCM;
    p.useGpuEncode = false; // ProRes encode is CPU-bound in FFmpeg builds
    return p;
}

EncodeProfile EncodeProfile::socialVertical() {
    EncodeProfile p;
    p.container = Container::MP4;
    p.videoCodec = VideoCodec::H264;
    p.width = 1080; p.height = 1920;
    p.videoBitrateKbps = 10000;
    return p;
}

} // namespace ds::exportmod
