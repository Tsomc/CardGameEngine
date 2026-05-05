#include "animation_system.h"
#include "core_log.h"
#include "math_utils.h"
#include <algorithm>

AnimationSystem::AnimationSystem()
    : nextId(1)
    , shakeIntensity(0.0f)
    , shakeDuration(0.0f)
    , shakeElapsed(0.0f)
    , shakeFrequency(30.0f)
{
}

AnimationSystem::~AnimationSystem()
{
}

bool AnimationSystem::ModuleInit()
{
    LOG_INFO("Animation", "动画系统初始化");
    return true;
}

void AnimationSystem::ModuleStart()
{
}

void AnimationSystem::ModuleUpdate(float deltaTime)
{
    AnimationUpdate(deltaTime);
}

void AnimationSystem::ModuleShutdown()
{
    activeTweens.clear();
    playingSequences.clear();
    cardOverrides.clear();
    uiOverrides.clear();
    LOG_INFO("Animation", "动画系统关闭");
}

const char* AnimationSystem::ModuleGetName() const
{
    return "AnimationSystem";
}

uint32_t AnimationSystem::AllocId()
{
    return nextId++;
}

void AnimationSystem::AnimationUpdate(float deltaTime)
{
    /* Update tweens */
    std::vector<uint32_t> toRemove;

    for (auto& pair : activeTweens) {
        TweenInstance& tween = pair.second;
        if (tween.completed) {
            toRemove.push_back(pair.first);
            continue;
        }

        tween.elapsed += deltaTime;
        float t = std::min(tween.elapsed / tween.duration, 1.0f);
        float easedT = g_easingTable[(uint8_t)tween.easing](t);

        Vec2 value = Vec2Lerp(tween.fromValue, tween.toValue, easedT);

        if (tween.cardTarget != INVALID_CARD_INSTANCE_ID) {
            AnimationOverride& ov = cardOverrides[tween.cardTarget];
            switch (tween.property) {
                case AnimationProperty::ANIM_PROP_POSITION:
                    ov.position = value;
                    break;
                case AnimationProperty::ANIM_PROP_SCALE:
                    ov.scale = value;
                    break;
                case AnimationProperty::ANIM_PROP_ROTATION:
                    ov.rotation = value.x;
                    break;
                case AnimationProperty::ANIM_PROP_ALPHA:
                    ov.alpha = value.x;
                    break;
                default:
                    break;
            }
        } else if (tween.uiTarget != 0) {
            AnimationOverride& ov = uiOverrides[tween.uiTarget];
            switch (tween.property) {
                case AnimationProperty::ANIM_PROP_POSITION:
                    ov.position = value;
                    break;
                case AnimationProperty::ANIM_PROP_SCALE:
                    ov.scale = value;
                    break;
                default:
                    break;
            }
        }

        if (t >= 1.0f) {
            tween.completed = true;
            if (tween.onComplete) {
                tween.onComplete();
            }
            toRemove.push_back(pair.first);

            if (tween.cardTarget != INVALID_CARD_INSTANCE_ID) {
                auto ovIt = cardOverrides.find(tween.cardTarget);
                if (ovIt != cardOverrides.end()) {
                    AnimationOverride& ov = ovIt->second;
                    switch (tween.property) {
                        case AnimationProperty::ANIM_PROP_POSITION:
                            ov.position = Vec2(0.0f, 0.0f);
                            break;
                        case AnimationProperty::ANIM_PROP_SCALE:
                            ov.scale = Vec2(1.0f, 1.0f);
                            break;
                        case AnimationProperty::ANIM_PROP_ROTATION:
                            ov.rotation = 0.0f;
                            break;
                        case AnimationProperty::ANIM_PROP_ALPHA:
                            ov.alpha = 1.0f;
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }

    for (uint32_t id : toRemove) {
        activeTweens.erase(id);
    }

    /* Update sequences */
    std::vector<uint32_t> seqToRemove;
    for (auto& pair : playingSequences) {
        SequenceInstance& seq = pair.second;
        if (seq.completed) {
            seqToRemove.push_back(pair.first);
            continue;
        }

        seq.clipElapsed += deltaTime;

        while (seq.clipIndex < seq.clips.size()) {
            AnimationClip& clip = seq.clips[seq.clipIndex];

            if (seq.clipElapsed < clip.delay) {
                break;
            }

            float clipT = seq.clipElapsed - clip.delay;
            if (clipT >= clip.duration) {
                if (clip.onKeyframe) {
                    clip.onKeyframe();
                }
                seq.clipIndex++;
                seq.clipElapsed = seq.clipElapsed - clip.delay - clip.duration;

                if (seq.clipIndex >= seq.clips.size()) {
                    if (seq.loop) {
                        seq.clipIndex = 0;
                        seq.clipElapsed = 0.0f;
                    } else {
                        seq.completed = true;
                        seqToRemove.push_back(pair.first);
                    }
                }
            } else {
                break;
            }
        }
    }

    for (uint32_t id : seqToRemove) {
        playingSequences.erase(id);
    }

    /* Update camera shake */
    if (shakeDuration > 0.0f) {
        shakeElapsed += deltaTime;
        if (shakeElapsed >= shakeDuration) {
            shakeIntensity = 0.0f;
            shakeDuration = 0.0f;
            shakeElapsed = 0.0f;
        }
    }
}

uint32_t AnimationSystem::AnimationPlayCardTween(
    CardInstanceId targetCardId,
    AnimationProperty property,
    const Vec2& fromValue,
    const Vec2& toValue,
    float duration,
    EasingType easing,
    std::function<void()> onComplete)
{
    TweenInstance tween;
    tween.id = AllocId();
    tween.cardTarget = targetCardId;
    tween.uiTarget = 0;
    tween.property = property;
    tween.fromValue = fromValue;
    tween.toValue = toValue;
    tween.duration = duration;
    tween.elapsed = 0.0f;
    tween.easing = easing;
    tween.onComplete = onComplete;
    tween.completed = false;

    activeTweens[tween.id] = tween;
    return tween.id;
}

uint32_t AnimationSystem::AnimationPlayUITween(
    uint32_t targetUIElementId,
    AnimationProperty property,
    const Vec2& fromValue,
    const Vec2& toValue,
    float duration,
    EasingType easing,
    std::function<void()> onComplete)
{
    TweenInstance tween;
    tween.id = AllocId();
    tween.cardTarget = INVALID_CARD_INSTANCE_ID;
    tween.uiTarget = targetUIElementId;
    tween.property = property;
    tween.fromValue = fromValue;
    tween.toValue = toValue;
    tween.duration = duration;
    tween.elapsed = 0.0f;
    tween.easing = easing;
    tween.onComplete = onComplete;
    tween.completed = false;

    activeTweens[tween.id] = tween;
    return tween.id;
}

uint32_t AnimationSystem::AnimationPlaySequence(
    CardInstanceId targetCardId,
    uint32_t targetUIId,
    const std::vector<AnimationClip>& clips,
    bool loop)
{
    SequenceInstance seq;
    seq.id = AllocId();
    seq.clipIndex = 0;
    seq.clipElapsed = 0.0f;
    seq.clips = clips;
    seq.loop = loop;
    seq.completed = false;

    playingSequences[seq.id] = seq;
    return seq.id;
}

uint32_t AnimationSystem::AnimationDelay(
    float delay, std::function<void()> callback)
{
    return AnimationPlayCardTween(
        INVALID_CARD_INSTANCE_ID,
        AnimationProperty::ANIM_PROP_POSITION,
        Vec2(0, 0),
        Vec2(0, 0),
        delay,
        EasingType::EASE_LINEAR,
        callback
    );
}

void AnimationSystem::AnimationCancel(uint32_t animationId)
{
    activeTweens.erase(animationId);
    playingSequences.erase(animationId);
}

void AnimationSystem::AnimationCancelAllOnCard(CardInstanceId cardId)
{
    std::vector<uint32_t> toRemove;
    for (auto& pair : activeTweens) {
        if (pair.second.cardTarget == cardId) {
            toRemove.push_back(pair.first);
        }
    }
    for (uint32_t id : toRemove) {
        activeTweens.erase(id);
    }
    cardOverrides.erase(cardId);
}

void AnimationSystem::AnimationCancelAllOnUI(uint32_t uiElementId)
{
    std::vector<uint32_t> toRemove;
    for (auto& pair : activeTweens) {
        if (pair.second.uiTarget == uiElementId) {
            toRemove.push_back(pair.first);
        }
    }
    for (uint32_t id : toRemove) {
        activeTweens.erase(id);
    }
    uiOverrides.erase(uiElementId);
}

bool AnimationSystem::AnimationIsPlaying(uint32_t animationId) const
{
    return activeTweens.find(animationId) != activeTweens.end() ||
           playingSequences.find(animationId) != playingSequences.end();
}

Vec2 AnimationSystem::AnimationGetCardPosition(CardInstanceId cardId) const
{
    auto it = cardOverrides.find(cardId);
    if (it != cardOverrides.end()) {
        return it->second.position;
    }
    return Vec2(0.0f, 0.0f);
}

Vec2 AnimationSystem::AnimationGetCardScale(CardInstanceId cardId) const
{
    auto it = cardOverrides.find(cardId);
    if (it != cardOverrides.end()) {
        return it->second.scale;
    }
    return Vec2(1.0f, 1.0f);
}

float AnimationSystem::AnimationGetCardRotation(CardInstanceId cardId) const
{
    auto it = cardOverrides.find(cardId);
    if (it != cardOverrides.end()) {
        return it->second.rotation;
    }
    return 0.0f;
}

float AnimationSystem::AnimationGetCardAlpha(CardInstanceId cardId) const
{
    auto it = cardOverrides.find(cardId);
    if (it != cardOverrides.end()) {
        return it->second.alpha;
    }
    return 1.0f;
}

uint32_t AnimationSystem::AnimationPlayCardEnter(
    CardInstanceId cardId, const Vec2& targetPos)
{
    Vec2 offScreen(-100.0f, -100.0f);
    return AnimationPlayCardTween(
        cardId,
        AnimationProperty::ANIM_PROP_POSITION,
        offScreen,
        targetPos,
        0.3f,
        EasingType::EASE_OUT_BACK
    );
}

uint32_t AnimationSystem::AnimationPlayCardExit(CardInstanceId cardId)
{
    return AnimationPlayCardTween(
        cardId,
        AnimationProperty::ANIM_PROP_POSITION,
        Vec2(0.0f, 0.0f),
        Vec2(-100.0f, -100.0f),
        0.25f,
        EasingType::EASE_IN_BACK
    );
}

uint32_t AnimationSystem::AnimationPlayCardShake(
    CardInstanceId cardId, float intensity, float duration)
{
    (void)cardId;
    (void)intensity;
    (void)duration;
    return 0;
}

uint32_t AnimationSystem::AnimationPlayCardFadeOut(
    CardInstanceId cardId, float duration)
{
    return AnimationPlayCardTween(
        cardId,
        AnimationProperty::ANIM_PROP_ALPHA,
        Vec2(1.0f, 0.0f),
        Vec2(0.0f, 0.0f),
        duration,
        EasingType::EASE_OUT_QUAD
    );
}

uint32_t AnimationSystem::AnimationPlayButtonPress(uint32_t uiElementId)
{
    return AnimationPlayUITween(
        uiElementId,
        AnimationProperty::ANIM_PROP_SCALE,
        Vec2(1.0f, 1.0f),
        Vec2(0.95f, 0.95f),
        0.05f,
        EasingType::EASE_OUT_QUAD
    );
}

uint32_t AnimationSystem::AnimationPlayButtonRelease(uint32_t uiElementId)
{
    return AnimationPlayUITween(
        uiElementId,
        AnimationProperty::ANIM_PROP_SCALE,
        Vec2(0.95f, 0.95f),
        Vec2(1.0f, 1.0f),
        0.05f,
        EasingType::EASE_OUT_QUAD
    );
}

void AnimationSystem::AnimationPlayCameraShake(
    float intensity, float duration, float frequency)
{
    shakeIntensity = intensity;
    shakeDuration = duration;
    shakeElapsed = 0.0f;
    shakeFrequency = frequency;
}
