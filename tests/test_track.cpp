#include <gtest/gtest.h>
#include "core/Track.h"

using namespace ds::core;

TEST(Track, ClipAtFindsCorrectClip) {
    Track track("t1", TrackType::Video, 0);
    auto c1 = std::make_unique<Clip>("c1", ClipType::Video, "m0");
    c1->setTrim({.inPoint = 0, .outPoint = 49}); // duration 50
    c1->moveTo(0);
    track.addClip(std::move(c1));

    auto c2 = std::make_unique<Clip>("c2", ClipType::Video, "m1");
    c2->setTrim({.inPoint = 0, .outPoint = 49});
    c2->moveTo(50);
    track.addClip(std::move(c2));

    EXPECT_EQ(track.clipAt(25)->id(), "c1");
    EXPECT_EQ(track.clipAt(75)->id(), "c2");
    EXPECT_EQ(track.clipAt(200), nullptr);
}

TEST(Track, MagneticTimelineClosesGapsOnRemove) {
    Track track("t1", TrackType::Video, 0);
    track.setMagnetic(true);

    auto c1 = std::make_unique<Clip>("c1", ClipType::Video, "m0");
    c1->setTrim({.inPoint = 0, .outPoint = 49});
    c1->moveTo(0);
    track.addClip(std::move(c1));

    auto c2 = std::make_unique<Clip>("c2", ClipType::Video, "m1");
    c2->setTrim({.inPoint = 0, .outPoint = 49});
    c2->moveTo(100); // gap between 50 and 100
    track.addClip(std::move(c2));

    track.removeClip("c1");
    ASSERT_EQ(track.clips().size(), 1u);
    EXPECT_EQ(track.clips()[0]->timelineStart(), 0); // gap closed
}
