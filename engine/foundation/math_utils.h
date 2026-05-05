#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include "math_types.h"

Vec2 Vec2Add(const Vec2& a, const Vec2& b);
Vec2 Vec2Sub(const Vec2& a, const Vec2& b);
Vec2 Vec2Scale(const Vec2& v, float s);
Vec2 Vec2Lerp(const Vec2& a, const Vec2& b, float t);
float Vec2Distance(const Vec2& a, const Vec2& b);
Vec2 Vec2Normalize(const Vec2& v);

bool RectContainsPoint(const Rect& r, const Vec2& p);
bool RectOverlaps(const Rect& a, const Rect& b);
Vec2 RectCenter(const Rect& r);

#endif
