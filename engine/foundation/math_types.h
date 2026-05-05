#ifndef MATH_TYPES_H
#define MATH_TYPES_H

#include <cstdint>

struct Vec2 {
    float x;
    float y;

    Vec2() : x(0.0f), y(0.0f) {}
    Vec2(float x, float y) : x(x), y(y) {}
};

struct Rect {
    float x;
    float y;
    float w;
    float h;

    Rect() : x(0.0f), y(0.0f), w(0.0f), h(0.0f) {}
    Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
};

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    Color() : r(255), g(255), b(255), a(255) {}
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}

    static const Color COLOR_WHITE;
    static const Color COLOR_BLACK;
    static const Color COLOR_RED;
    static const Color COLOR_GREEN;
    static const Color COLOR_BLUE;
    static const Color COLOR_YELLOW;
    static const Color COLOR_TRANSPARENT;
};

#endif
