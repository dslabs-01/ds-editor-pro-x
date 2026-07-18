#include <gtest/gtest.h>
#include "core/Timeline.h"

using namespace ds::core;

TEST(Timeline, DurationFramesReflectsLongestClip) {
    Timeline tl("tl1", "Test", 30, 1, 1920, 1080);
    Track* v1 = tl.addTrack(TrackType::Video);
    auto c1 = std::make_unique<Clip>("c1", ClipType::Video, "m0");
    c1->setTrim({.inPoint = 0, .outPoint = 149}); // 150 frames = 5s @30fps
    c1->moveTo(0);
    v1->addClip(std::move(c1));

    EXPECT_EQ(tl.durationFrames(), 150);
    EXPECT_DOUBLE_EQ(tl.frameRate(), 30.0);
}

TEST(Timeline, AddTrackAssignsUniqueIds) {
    Timeline tl("tl1", "Test", 30, 1, 1920, 1080);
    Track* v1 = tl.addTrack(TrackType::Video);
    Track* v2 = tl.addTrack(TrackType::Video);
    Track* a1 = tl.addTrack(TrackType::Audio);
    EXPECT_NE(v1->id(), v2->id());
    EXPECT_NE(v2->id(), a1->id());
    EXPECT_EQ(tl.tracks().size(), 3u);
}
