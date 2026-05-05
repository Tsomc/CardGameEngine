#ifndef ANIMATION_SYSTEM_H
#define ANIMATION_SYSTEM_H

#include "module_manager.h"
#include "animation_clip.h"
#include "easing.h"
#include "math_types.h"
#include <unordered_map>
#include <vector>
#include <functional>
#include <cstdint>

#include "card.h"

struct TweenInstance {
    uint32_t id;
    CardInstanceId cardTarget;
    uint32_t uiTarget;
    AnimationProperty property;
    Vec2 fromValue;
    Vec2 toValue;
    float duration;
    float elapsed;
    EasingType easing;
    std::function<void()> onComplete;
    bool completed;
};

struct SequenceInstance {
    uint32_t id;
    uint32_t clipIndex;
    float clipElapsed;
    std::vector<AnimationClip> clips;
    bool loop;
    bool completed;
};

class AnimationSystem : public IModule {
public:
    AnimationSystem();
    ~AnimationSystem() override;

    bool ModuleInit() override;
    void ModuleStart() override;
    void ModuleUpdate(float deltaTime) override;
    void ModuleShutdown() override;
    const char* ModuleGetName() const override;

    void AnimationUpdate(float deltaTime);

    uint32_t AnimationPlayCardTween(
        CardInstanceId targetCardId,
        AnimationProperty property,
        const Vec2& fromValue,
        const Vec2& toValue,
        float duration,
        EasingType easing,
        std::function<void()> onComplete = nullptr);

    uint32_t AnimationPlayUITween(
        uint32_t targetUIElementId,
        AnimationProperty property,
        const Vec2& fromValue,
        const Vec2& toValue,
        float duration,
        EasingType easing,
        std::function<void()> onComplete = nullptr);

    uint32_t AnimationPlaySequence(
        CardInstanceId targetCardId,
        uint32_t targetUIId,
        const std::vector<AnimationClip>& clips,
        bool loop = false);

    uint32_t AnimationDelay(
        float delay, std::function<void()> callback);

    void AnimationCancel(uint32_t animationId);
    void AnimationCancelAllOnCard(CardInstanceId cardId);
    void AnimationCancelAllOnUI(uint32_t uiElementId);
    bool AnimationIsPlaying(uint32_t animationId) const;

    Vec2 AnimationGetCardPosition(CardInstanceId cardId) const;
    Vec2 AnimationGetCardScale(CardInstanceId cardId) const;
    float AnimationGetCardRotation(CardInstanceId cardId) const;
    float AnimationGetCardAlpha(CardInstanceId cardId) const;

    /* 内置运动模式 */
    uint32_t AnimationPlayCardEnter(
        CardInstanceId cardId, const Vec2& targetPos);
    uint32_t AnimationPlayCardExit(CardInstanceId cardId);
    uint32_t AnimationPlayCardShake(
        CardInstanceId cardId, float intensity = 5.0f, float duration = 0.3f);
    uint32_t AnimationPlayCardFadeOut(
        CardInstanceId cardId, float duration = 0.5f);
    uint32_t AnimationPlayButtonPress(uint32_t uiElementId);
    uint32_t AnimationPlayButtonRelease(uint32_t uiElementId);
    void AnimationPlayCameraShake(
        float intensity, float duration, float frequency = 30.0f);

private:
    uint32_t AllocId();

    std::unordered_map<uint32_t, TweenInstance> activeTweens;
    std::unordered_map<uint32_t, SequenceInstance> playingSequences;
    std::unordered_map<CardInstanceId, AnimationOverride> cardOverrides;
    std::unordered_map<uint32_t, AnimationOverride> uiOverrides;
    uint32_t nextId;

    /* Camera shake state */
    float shakeIntensity;
    float shakeDuration;
    float shakeElapsed;
    float shakeFrequency;
};

#endif
