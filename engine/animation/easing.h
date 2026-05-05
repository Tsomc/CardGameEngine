#ifndef EASING_H
#define EASING_H

#include <cstdint>

enum class EasingType : uint8_t {
    EASE_LINEAR         = 0,
    EASE_IN_QUAD        = 1,
    EASE_OUT_QUAD       = 2,
    EASE_INOUT_QUAD     = 3,
    EASE_IN_CUBIC       = 4,
    EASE_OUT_CUBIC      = 5,
    EASE_INOUT_CUBIC    = 6,
    EASE_IN_BACK        = 7,
    EASE_OUT_BACK       = 8,
    EASE_INOUT_BACK     = 9,
    EASE_OUT_BOUNCE     = 10,
    EASE_OUT_ELASTIC    = 11
};

using EasingFunction = float(*)(float);

float EaseLinear(float t);
float EaseInQuad(float t);
float EaseOutQuad(float t);
float EaseInOutQuad(float t);
float EaseInCubic(float t);
float EaseOutCubic(float t);
float EaseInOutCubic(float t);
float EaseInBack(float t);
float EaseOutBack(float t);
float EaseInOutBack(float t);
float EaseOutBounce(float t);
float EaseOutElastic(float t);

extern EasingFunction g_easingTable[12];

#endif
