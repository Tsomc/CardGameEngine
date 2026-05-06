#include "card_hand_layout.h"
#include <algorithm>
#include <cmath>
using namespace engine;
std::vector<CardHandPose> CardHandLayout::CardHandRecalculate(
    const std::vector<Card*>& cards,
    const Vec2& arcCenter,
    float arcRadius,
    float arcAngle,
    float baseRotation)
{
    std::vector<CardHandPose> poses;
    int N = (int)cards.size();

    if (N == 0) {
        return poses;
    }

    poses.resize(N);

    if (N == 1) {
        poses[0].position = Vec2(arcCenter.x, arcCenter.y - arcRadius);
        poses[0].rotation = baseRotation;
        return poses;
    }

    float angleStep = arcAngle / (N - 1);
    float startAngle = -(arcAngle / 2.0f);

    for (int i = 0; i < N; ++i) {
        float theta = startAngle + i * angleStep;
        poses[i].position.x = arcCenter.x + arcRadius * std::sin(theta);
        poses[i].position.y = arcCenter.y - arcRadius * std::cos(theta);
        poses[i].rotation = theta + baseRotation;
    }

    return poses;
}

Vec2 CardHandLayout::CardHandGetHoverOffset(
    const CardHandPose& cardPose, float hoverOffsetY)
{
    return Vec2(cardPose.position.x, cardPose.position.y + hoverOffsetY);
}

float CardHandLayout::CardHandCalcAngleStep(
    int cardCount, float cardWidth, float arcRadius, float maxSpacing)
{
    if (cardCount <= 1 || arcRadius < 0.001f) {
        return 0.0f;
    }

    float spacing = std::min(cardWidth + 10.0f, maxSpacing);
    float angleStep = spacing / arcRadius;
    float maxAngleStep = maxSpacing / arcRadius;

    return std::min(angleStep, maxAngleStep);
}
