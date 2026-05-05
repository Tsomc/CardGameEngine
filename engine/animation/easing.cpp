#include "easing.h"
#include <cmath>

EasingFunction g_easingTable[12] = {
    EaseLinear,
    EaseInQuad, EaseOutQuad, EaseInOutQuad,
    EaseInCubic, EaseOutCubic, EaseInOutCubic,
    EaseInBack, EaseOutBack, EaseInOutBack,
    EaseOutBounce, EaseOutElastic
};

float EaseLinear(float t)
{
    return t;
}

float EaseInQuad(float t)
{
    return t * t;
}

float EaseOutQuad(float t)
{
    return t * (2.0f - t);
}

float EaseInOutQuad(float t)
{
    if (t < 0.5f) {
        return 2.0f * t * t;
    }
    return -1.0f + (4.0f - 2.0f * t) * t;
}

float EaseInCubic(float t)
{
    return t * t * t;
}

float EaseOutCubic(float t)
{
    float t1 = t - 1.0f;
    return t1 * t1 * t1 + 1.0f;
}

float EaseInOutCubic(float t)
{
    if (t < 0.5f) {
        return 4.0f * t * t * t;
    }
    float t1 = 2.0f * t - 2.0f;
    return 0.5f * t1 * t1 * t1 + 1.0f;
}

float EaseInBack(float t)
{
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;
    return c3 * t * t * t - c1 * t * t;
}

float EaseOutBack(float t)
{
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;
    float t1 = t - 1.0f;
    return 1.0f + c3 * t1 * t1 * t1 + c1 * t1 * t1;
}

float EaseInOutBack(float t)
{
    const float c1 = 1.70158f;
    const float c2 = c1 * 1.525f;

    if (t < 0.5f) {
        float t1 = 2.0f * t;
        return (t1 * t1 * (c2 + 1.0f) * t1 - c2 * t1 * t1) / 2.0f;
    }
    float t1 = 2.0f * t - 2.0f;
    return (t1 * t1 * (c2 + 1.0f) * t1 + c2 * t1 * t1 + 2.0f) / 2.0f;
}

float EaseOutBounce(float t)
{
    if (t < 1.0f / 2.75f) {
        return 7.5625f * t * t;
    } else if (t < 2.0f / 2.75f) {
        float t1 = t - 1.5f / 2.75f;
        return 7.5625f * t1 * t1 + 0.75f;
    } else if (t < 2.5f / 2.75f) {
        float t1 = t - 2.25f / 2.75f;
        return 7.5625f * t1 * t1 + 0.9375f;
    } else {
        float t1 = t - 2.625f / 2.75f;
        return 7.5625f * t1 * t1 + 0.984375f;
    }
}

float EaseOutElastic(float t)
{
    if (t < 0.001f) return 0.0f;
    if (t > 0.999f) return 1.0f;

    return std::pow(2.0f, -10.0f * t) *
           std::sin((t - 0.075f) * 2.0f * 3.14159265f / 0.3f) + 1.0f;
}
