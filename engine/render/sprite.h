#ifndef SPRITE_H
#define SPRITE_H

#include "math_types.h"
#include "resource_handle.h"
#include <cstdint>

enum class FlipMode : uint8_t {
    FLIP_NONE        = 0,
    FLIP_HORIZONTAL  = 1,
    FLIP_VERTICAL    = 2
};

struct Sprite {
    ResourceId textureId;
    Rect srcRect;
    Rect dstRect;
    Color colorMod;
    float angle;
    Vec2 pivot;
    FlipMode flip;
    int layer;

    Sprite()
        : textureId(INVALID_RESOURCE_ID)
        , colorMod(255, 255, 255, 255)
        , angle(0.0f)
        , pivot(0.0f, 0.0f)
        , flip(FlipMode::FLIP_NONE)
        , layer(0)
    {
    }
};

#endif
