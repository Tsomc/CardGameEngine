#include "ui_panel.h"
#include <algorithm>

void UIPanel::UIPanelAddChild(UIElement* child)
{
    if (child == nullptr) {
        return;
    }
    children.push_back(std::unique_ptr<UIElement>(child));
}

void UIPanel::UIPanelRemoveChild(uint32_t childId)
{
    children.erase(
        std::remove_if(children.begin(), children.end(),
            [childId](const std::unique_ptr<UIElement>& elem) {
                return elem->id == childId;
            }),
        children.end()
    );
}

UIElement* UIPanel::UIPanelGetChild(uint32_t childId)
{
    for (auto& elem : children) {
        if (elem->id == childId) {
            return elem.get();
        }
    }
    return nullptr;
}

void UIPanel::UIPanelClear()
{
    children.clear();
}

void UIPanel::UIUpdate(float deltaTime)
{
    for (auto& child : children) {
        if (child->visible) {
            child->UIUpdate(deltaTime);
        }
    }
}

void UIPanel::UIRender()
{
    for (auto& child : children) {
        if (child->visible) {
            child->UIRender();
        }
    }
}

void UIPanel::UIHandleEvent(const Event& event)
{
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        if ((*it)->visible && (*it)->enabled) {
            (*it)->UIHandleEvent(event);
        }
    }
}
