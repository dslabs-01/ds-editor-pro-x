#include <gtest/gtest.h>
#include "core/Clip.h"

using namespace ds::core;

TEST(Clip, DurationMatchesTrimHandles) {
    Clip clip("c1", ClipType::Video, "media_0");
    clip.setTrim({.inPoint = 10, .outPoint = 99});
    EXPECT_EQ(clip.duration(), 90);
}

TEST(Clip, SlipShiftsSourceWindowWithoutMovingTimelinePosition) {
    Clip clip("c1", ClipType::Video, "media_0");
    clip.moveTo(500);
    clip.setTrim({.inPoint = 10, .outPoint = 59});
    FrameTime before = clip.timelineStart();
    clip.slip(5);
    EXPECT_EQ(clip.timelineStart(), before); // slip must not move timeline position
    EXPECT_EQ(clip.trim().inPoint, 15);
    EXPECT_EQ(clip.trim().outPoint, 64);
}

TEST(Clip, TrimInRollsTimelineStartWithSourceIn) {
    Clip clip("c1", ClipType::Video, "media_0");
    clip.moveTo(100);
    clip.setTrim({.inPoint = 0, .outPoint = 99});
    clip.trimIn(10); // trim 10 frames off the head
    EXPECT_EQ(clip.trim().inPoint, 10);
    EXPECT_EQ(clip.timelineStart(), 110);
}

TEST(Clip, TrimOutCannotGoBeforeInPoint) {
    Clip clip("c1", ClipType::Video, "media_0");
    clip.setTrim({.inPoint = 20, .outPoint = 50});
    clip.trimOut(5); // invalid: before inPoint
    EXPECT_GT(clip.trim().outPoint, clip.trim().inPoint);
}
