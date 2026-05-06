#include <gtest/gtest.h>
#include "easing.h"

using namespace engine;

TEST(EasingTest, Linear)
{
    EXPECT_FLOAT_EQ(g_easingTable[0](0.0f), 0.0f);
    EXPECT_FLOAT_EQ(g_easingTable[0](0.5f), 0.5f);
    EXPECT_FLOAT_EQ(g_easingTable[0](1.0f), 1.0f);
}

TEST(EasingTest, InQuad)
{
    float f = g_easingTable[1](0.5f);
    EXPECT_FLOAT_EQ(f, 0.25f);
    EXPECT_FLOAT_EQ(g_easingTable[1](0.0f), 0.0f);
    EXPECT_FLOAT_EQ(g_easingTable[1](1.0f), 1.0f);
}

TEST(EasingTest, OutQuad)
{
    float f = g_easingTable[2](0.5f);
    EXPECT_FLOAT_EQ(f, 0.75f);
    EXPECT_FLOAT_EQ(g_easingTable[2](0.0f), 0.0f);
    EXPECT_FLOAT_EQ(g_easingTable[2](1.0f), 1.0f);
}

TEST(EasingTest, InCubic)
{
    float f = g_easingTable[4](0.5f);
    EXPECT_FLOAT_EQ(f, 0.125f);
    EXPECT_FLOAT_EQ(g_easingTable[4](0.0f), 0.0f);
    EXPECT_FLOAT_EQ(g_easingTable[4](1.0f), 1.0f);
}

TEST(EasingTest, OutCubic)
{
    float f = g_easingTable[5](0.5f);
    EXPECT_FLOAT_EQ(f, 0.875f);
}

TEST(EasingTest, OutBounce)
{
    float f = g_easingTable[10](0.5f);
    EXPECT_GT(f, 0.0f);
    EXPECT_LT(f, 1.0f);
    EXPECT_FLOAT_EQ(g_easingTable[10](0.0f), 0.0f);
    EXPECT_NEAR(g_easingTable[10](1.0f), 1.0f, 0.01f);
}

TEST(EasingTest, OutElastic)
{
    EXPECT_FLOAT_EQ(g_easingTable[11](0.0f), 0.0f);
    EXPECT_NEAR(g_easingTable[11](1.0f), 1.0f, 0.01f);
}

TEST(EasingTest, InOutBack)
{
    EXPECT_FLOAT_EQ(g_easingTable[9](0.0f), 0.0f);
    EXPECT_NEAR(g_easingTable[9](1.0f), 1.0f, 0.01f);
}

TEST(EasingTest, RangeCheck)
{
    for (int easing = 0; easing < 12; ++easing) {
        EXPECT_GE(g_easingTable[easing](0.0f), -0.001f);
        EXPECT_LE(g_easingTable[easing](1.0f), 1.001f);
    }
}

TEST(EasingTest, Range)
{
    for (int easing = 0; easing < 12; ++easing) {
        EXPECT_FLOAT_EQ(g_easingTable[easing](0.0f), 0.0f);
        EXPECT_NEAR(g_easingTable[easing](1.0f), 1.0f, 0.01f);
    }
}
