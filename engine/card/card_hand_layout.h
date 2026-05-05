#ifndef CARD_HAND_LAYOUT_H
#define CARD_HAND_LAYOUT_H

#include "math_types.h"
#include "card.h"
#include <vector>

struct CardHandPose {
    Vec2 position;
    float rotation;
};

class CardHandLayout {
public:
    std::vector<CardHandPose> CardHandRecalculate(
        const std::vector<Card*>& cards,
        const Vec2& arcCenter,
        float arcRadius,
        float arcAngle,
        float baseRotation = 0.0f);

    Vec2 CardHandGetHoverOffset(
        const CardHandPose& cardPose,
        float hoverOffsetY = -30.0f);

    float CardHandCalcAngleStep(
        int cardCount,
        float cardWidth,
        float arcRadius,
        float maxSpacing = 80.0f);
};

#endif
