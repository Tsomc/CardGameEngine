#ifndef CAMERA_H
#define CAMERA_H

#include "math_types.h"


namespace engine {

struct Camera {
    Vec2 position;
    float zoom;
    float angle;
    Rect bounds;

    Camera()
        : zoom(1.0f)
        , angle(0.0f)
    {
    }

    Vec2 CameraScreenToWorld(
        const Vec2& screenPos, int screenWidth, int screenHeight) const;
    Vec2 CameraWorldToScreen(
        const Vec2& worldPos, int screenWidth, int screenHeight) const;
};

namespace CameraOps {
    void CameraSetBounds(Camera& cam, const Rect& bounds);
    void CameraLookAt(Camera& cam, const Vec2& target);
    void CameraSmoothFollow(
        Camera& cam, const Vec2& target, float smoothSpeed, float deltaTime);
    Rect CameraGetViewport(
        const Camera& cam, int screenWidth, int screenHeight);
}


} /* namespace engine */
#endif
