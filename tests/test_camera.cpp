#include <gtest/gtest.h>
#include "camera.h"
#include "math_types.h"

using namespace engine;

TEST(CameraTest, DefaultState)
{
    Camera cam;
    EXPECT_FLOAT_EQ(cam.position.x, 0.0f);
    EXPECT_FLOAT_EQ(cam.position.y, 0.0f);
    EXPECT_FLOAT_EQ(cam.zoom, 1.0f);
    EXPECT_FLOAT_EQ(cam.angle, 0.0f);
}

TEST(CameraTest, WorldToScreen)
{
    Camera cam;
    cam.position = Vec2(100.0f, 100.0f);

    Vec2 screen = cam.CameraWorldToScreen(Vec2(200.0f, 200.0f), 1280, 720);
    EXPECT_FLOAT_EQ(screen.x, 1280.0f / 2.0f + 100.0f);
    EXPECT_FLOAT_EQ(screen.y, 720.0f / 2.0f + 100.0f);
}

TEST(CameraTest, ScreenToWorld)
{
    Camera cam;
    cam.position = Vec2(100.0f, 100.0f);

    Vec2 world = cam.CameraScreenToWorld(Vec2(740.0f, 460.0f), 1280, 720);
    EXPECT_FLOAT_EQ(world.x, 200.0f);
    EXPECT_FLOAT_EQ(world.y, 200.0f);
}

TEST(CameraTest, Zoom)
{
    Camera cam;
    cam.position = Vec2(0.0f, 0.0f);
    cam.zoom = 2.0f;

    Vec2 screen = cam.CameraWorldToScreen(Vec2(100.0f, 100.0f), 1280, 720);
    EXPECT_FLOAT_EQ(screen.x, 1280.0f / 2.0f + 200.0f);
    EXPECT_FLOAT_EQ(screen.y, 720.0f / 2.0f + 200.0f);
}

TEST(CameraTest, RoundTrip)
{
    Camera cam;
    cam.position = Vec2(50.0f, -30.0f);
    cam.zoom = 1.5f;

    Vec2 original(123.0f, 456.0f);
    Vec2 screen = cam.CameraWorldToScreen(original, 1280, 720);
    Vec2 world = cam.CameraScreenToWorld(screen, 1280, 720);

    EXPECT_NEAR(world.x, original.x, 0.01f);
    EXPECT_NEAR(world.y, original.y, 0.01f);
}

TEST(CameraTest, LookAtBounds)
{
    Camera cam;

    CameraOps::CameraLookAt(cam, Vec2(100.0f, 200.0f));
    EXPECT_FLOAT_EQ(cam.position.x, 100.0f);
    EXPECT_FLOAT_EQ(cam.position.y, 200.0f);
}

TEST(CameraTest, GetViewport)
{
    Camera cam;
    cam.position = Vec2(100.0f, 100.0f);

    Rect vp = CameraOps::CameraGetViewport(cam, 1280, 720);
    EXPECT_FLOAT_EQ(vp.x, 100.0f - 640.0f);
    EXPECT_FLOAT_EQ(vp.y, 100.0f - 360.0f);
    EXPECT_FLOAT_EQ(vp.w, 1280.0f);
    EXPECT_FLOAT_EQ(vp.h, 720.0f);
}

TEST(CameraTest, ZoomViewport)
{
    Camera cam;
    cam.zoom = 2.0f;

    Rect vp = CameraOps::CameraGetViewport(cam, 1280, 720);
    EXPECT_FLOAT_EQ(vp.w, 640.0f);
    EXPECT_FLOAT_EQ(vp.h, 360.0f);
}
