#include "camera.h"
#include "math_utils.h"
#include <algorithm>
using namespace engine;
Vec2 Camera::CameraScreenToWorld(
    const Vec2& screenPos, int screenWidth, int screenHeight) const
{
    float cx = screenWidth / 2.0f;
    float cy = screenHeight / 2.0f;
    float wx = (screenPos.x - cx) / zoom + position.x;
    float wy = (screenPos.y - cy) / zoom + position.y;
    return Vec2(wx, wy);
}

Vec2 Camera::CameraWorldToScreen(
    const Vec2& worldPos, int screenWidth, int screenHeight) const
{
    float cx = screenWidth / 2.0f;
    float cy = screenHeight / 2.0f;
    float sx = (worldPos.x - position.x) * zoom + cx;
    float sy = (worldPos.y - position.y) * zoom + cy;
    return Vec2(sx, sy);
}

void CameraOps::CameraSetBounds(Camera& cam, const Rect& bounds)
{
    cam.bounds = bounds;
}

void CameraOps::CameraLookAt(Camera& cam, const Vec2& target)
{
    Vec2 clamped = target;

    if (cam.bounds.w > 0.0f && cam.bounds.h > 0.0f) {
        float viewW = 0.0f;
        float viewH = 0.0f;

        if (cam.zoom > 0.0f) {
            viewW = 1280.0f / cam.zoom;
            viewH = 720.0f / cam.zoom;
        }

        clamped.x = std::max(cam.bounds.x,
            std::min(target.x, cam.bounds.x + cam.bounds.w - viewW));
        clamped.y = std::max(cam.bounds.y,
            std::min(target.y, cam.bounds.y + cam.bounds.h - viewH));
    }

    cam.position = clamped;
}

void CameraOps::CameraSmoothFollow(
    Camera& cam, const Vec2& target, float smoothSpeed, float deltaTime)
{
    float t = std::min(1.0f, smoothSpeed * deltaTime * 60.0f);
    Vec2 lerped = Vec2Lerp(cam.position, target, t);

    if (cam.bounds.w > 0.0f && cam.bounds.h > 0.0f) {
        float viewW = 1280.0f / cam.zoom;
        float viewH = 720.0f / cam.zoom;
        lerped.x = std::max(cam.bounds.x,
            std::min(lerped.x, cam.bounds.x + cam.bounds.w - viewW));
        lerped.y = std::max(cam.bounds.y,
            std::min(lerped.y, cam.bounds.y + cam.bounds.h - viewH));
    }

    cam.position = lerped;
}

Rect CameraOps::CameraGetViewport(
    const Camera& cam, int screenWidth, int screenHeight)
{
    float halfW = screenWidth / (2.0f * cam.zoom);
    float halfH = screenHeight / (2.0f * cam.zoom);

    return Rect(
        cam.position.x - halfW,
        cam.position.y - halfH,
        halfW * 2.0f,
        halfH * 2.0f
    );
}
