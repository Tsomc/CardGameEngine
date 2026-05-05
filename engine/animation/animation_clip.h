#ifndef ANIMATION_CLIP_H
#define ANIMATION_CLIP_H

#include "easing.h"
#include "math_types.h"
#include <functional>
#include <cstdint>

enum class AnimationProperty : uint8_t {
    ANIM_PROP_POSITION  = 0,
    ANIM_PROP_SCALE     = 1,
    ANIM_PROP_ROTATION  = 2,
    ANIM_PROP_ALPHA     = 3,
    ANIM_PROP_COLOR     = 4
};

struct AnimationClip {
    AnimationProperty property;
    Vec2 fromValue;
    Vec2 toValue;
    float duration;
    EasingType easing;
    float delay;
    std::function<void()> onKeyframe;
};

struct AnimationOverride {
    Vec2 position;
    Vec2 scale;
    float rotation;
    float alpha;

    AnimationOverride()
        : position(0.0f, 0.0f)
        , scale(1.0f, 1.0f)
        , rotation(0.0f)
        , alpha(1.0f)
    {
    }
};

#endif
