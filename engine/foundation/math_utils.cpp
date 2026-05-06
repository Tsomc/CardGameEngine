#include "math_utils.h"
#include <cmath>
using namespace engine;
Vec2 Vec2Add(const Vec2& a, const Vec2& b)
{
    return Vec2(a.x + b.x, a.y + b.y);
}

Vec2 Vec2Sub(const Vec2& a, const Vec2& b)
{
    return Vec2(a.x - b.x, a.y - b.y);
}

Vec2 Vec2Scale(const Vec2& v, float s)
{
    return Vec2(v.x * s, v.y * s);
}

Vec2 Vec2Lerp(const Vec2& a, const Vec2& b, float t)
{
    return Vec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
}

float Vec2Distance(const Vec2& a, const Vec2& b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return std::sqrt(dx * dx + dy * dy);
}

Vec2 Vec2Normalize(const Vec2& v)
{
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len < 0.0001f) {
        return Vec2(0.0f, 0.0f);
    }
    return Vec2(v.x / len, v.y / len);
}

bool RectContainsPoint(const Rect& r, const Vec2& p)
{
    return p.x >= r.x && p.x <= r.x + r.w
        && p.y >= r.y && p.y <= r.y + r.h;
}

bool RectOverlaps(const Rect& a, const Rect& b)
{
    return a.x < b.x + b.w && a.x + a.w > b.x
        && a.y < b.y + b.h && a.y + a.h > b.y;
}

Vec2 RectCenter(const Rect& r)
{
    return Vec2(r.x + r.w / 2.0f, r.y + r.h / 2.0f);
}
