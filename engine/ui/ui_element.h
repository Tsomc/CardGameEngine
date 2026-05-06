#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include "math_types.h"
#include "event_bus.h"
#include "card.h"
#include <string>
#include <cstdint>


namespace engine {

struct UIButtonClickedEvent : Event {
    uint32_t buttonId;
    std::string buttonName;
};

struct UITextFieldSubmitEvent : Event {
    uint32_t textFieldId;
    std::string text;
};

struct UICardSelectedEvent : Event {
    uint32_t cardId;
    uint8_t playerId;
};

struct UICardDeselectedEvent : Event {
    uint32_t cardId;
};

struct UICardDragBeginEvent : Event {
    uint32_t cardId;
    Vec2 startPosition;
};

struct UICardDragMoveEvent : Event {
    uint32_t cardId;
    Vec2 currentPosition;
    Vec2 delta;
};

struct UICardDragEndEvent : Event {
    uint32_t cardId;
    Vec2 endPosition;
    uint32_t targetCardId;
    ZoneType targetZone;
};

class UIElement {
public:
    UIElement();
    virtual ~UIElement() = default;

    uint32_t UIGetId() const;
    void UISetRect(const Rect& r);
    const Rect& UIGetRect() const;

    void UISetVisible(bool visible);
    bool UIIsVisible() const;
    void UISetEnabled(bool enabled);
    bool UIIsEnabled() const;
    int UIGetLayer() const;
    void UISetLayer(int layer);

    virtual bool UIContainsPoint(const Vec2& point) const;
    virtual void UIUpdate(float deltaTime);
    virtual void UIRender();
    virtual void UIHandleEvent(const Event& event);

public:
    uint32_t id;
    Rect rect;
    bool visible;
    bool enabled;
    int layer;

    friend class UISystem;
};


} /* namespace engine */
#endif
