#ifndef UI_PANEL_H
#define UI_PANEL_H

#include "ui_element.h"
#include <vector>
#include <memory>

class UIPanel : public UIElement {
public:
    void UIPanelAddChild(UIElement* child);
    void UIPanelRemoveChild(uint32_t childId);
    UIElement* UIPanelGetChild(uint32_t childId);
    void UIPanelClear();
    void UIUpdate(float deltaTime) override;
    void UIRender() override;
    void UIHandleEvent(const Event& event) override;

private:
    std::vector<std::unique_ptr<UIElement>> children;
};

#endif
