#include <gtest/gtest.h>
#include "math_types.h"
#include "math_utils.h"

using namespace engine;

TEST(MathTest, Vec2Add)
{
    Vec2 a(1.0f, 2.0f);
    Vec2 b(3.0f, 4.0f);
    Vec2 c = Vec2Add(a, b);
    EXPECT_FLOAT_EQ(c.x, 4.0f);
    EXPECT_FLOAT_EQ(c.y, 6.0f);
}

TEST(MathTest, Vec2Sub)
{
    Vec2 a(5.0f, 7.0f);
    Vec2 b(2.0f, 3.0f);
    Vec2 c = Vec2Sub(a, b);
    EXPECT_FLOAT_EQ(c.x, 3.0f);
    EXPECT_FLOAT_EQ(c.y, 4.0f);
}

TEST(MathTest, Vec2Scale)
{
    Vec2 v(2.0f, 3.0f);
    Vec2 s = Vec2Scale(v, 2.0f);
    EXPECT_FLOAT_EQ(s.x, 4.0f);
    EXPECT_FLOAT_EQ(s.y, 6.0f);
}

TEST(MathTest, Vec2Lerp)
{
    Vec2 a(0.0f, 0.0f);
    Vec2 b(10.0f, 20.0f);
    Vec2 m = Vec2Lerp(a, b, 0.5f);
    EXPECT_FLOAT_EQ(m.x, 5.0f);
    EXPECT_FLOAT_EQ(m.y, 10.0f);
}

TEST(MathTest, Vec2Distance)
{
    Vec2 a(0.0f, 0.0f);
    Vec2 b(3.0f, 4.0f);
    float d = Vec2Distance(a, b);
    EXPECT_FLOAT_EQ(d, 5.0f);
}

TEST(MathTest, RectContainsPoint)
{
    Rect r(0.0f, 0.0f, 100.0f, 100.0f);
    EXPECT_TRUE(RectContainsPoint(r, Vec2(50.0f, 50.0f)));
    EXPECT_TRUE(RectContainsPoint(r, Vec2(0.0f, 0.0f)));
    EXPECT_TRUE(RectContainsPoint(r, Vec2(100.0f, 100.0f)));
    EXPECT_FALSE(RectContainsPoint(r, Vec2(-1.0f, 50.0f)));
    EXPECT_FALSE(RectContainsPoint(r, Vec2(50.0f, 101.0f)));
}

TEST(MathTest, RectOverlaps)
{
    Rect a(0.0f, 0.0f, 100.0f, 100.0f);
    Rect b(50.0f, 50.0f, 100.0f, 100.0f);
    Rect c(200.0f, 200.0f, 50.0f, 50.0f);

    EXPECT_TRUE(RectOverlaps(a, b));
    EXPECT_FALSE(RectOverlaps(a, c));
}

TEST(MathTest, RectCenter)
{
    Rect r(10.0f, 20.0f, 100.0f, 200.0f);
    Vec2 c = RectCenter(r);
    EXPECT_FLOAT_EQ(c.x, 60.0f);
    EXPECT_FLOAT_EQ(c.y, 120.0f);
}

TEST(MathTest, ColorConstants)
{
    EXPECT_EQ(Color::COLOR_WHITE.r, 255);
    EXPECT_EQ(Color::COLOR_WHITE.g, 255);
    EXPECT_EQ(Color::COLOR_WHITE.b, 255);
    EXPECT_EQ(Color::COLOR_WHITE.a, 255);

    EXPECT_EQ(Color::COLOR_TRANSPARENT.a, 0);
}
