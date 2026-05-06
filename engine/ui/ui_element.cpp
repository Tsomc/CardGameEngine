#include "ui_element.h"
#include "math_utils.h"
using namespace engine;
UIElement::UIElement()
    : id(0)
    , visible(true)
    , enabled(true)
    , layer(0)
{
}

uint32_t UIElement::UIGetId() const
{
    return id;
}

void UIElement::UISetRect(const Rect& r)
{
    rect = r;
}

const Rect& UIElement::UIGetRect() const
{
    return rect;
}

void UIElement::UISetVisible(bool v)
{
    visible = v;
}

bool UIElement::UIIsVisible() const
{
    return visible;
}

void UIElement::UISetEnabled(bool e)
{
    enabled = e;
}

bool UIElement::UIIsEnabled() const
{
    return enabled;
}

int UIElement::UIGetLayer() const
{
    return layer;
}

void UIElement::UISetLayer(int l)
{
    layer = l;
}

bool UIElement::UIContainsPoint(const Vec2& point) const
{
    return RectContainsPoint(rect, point);
}

void UIElement::UIUpdate(float deltaTime)
{
    (void)deltaTime;
}

void UIElement::UIRender()
{
}

void UIElement::UIHandleEvent(const Event& event)
{
    (void)event;
}
