#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "ui_element.h"
#include "resource_handle.h"
#include <string>
#include <functional>

class UIButton : public UIElement {
public:
    UIButton();

    void UISetText(const std::string& text);
    void UISetFontSize(int size);
    void UISetNormalTexture(ResourceId texId);
    void UISetHoverTexture(ResourceId texId);
    void UISetPressedTexture(ResourceId texId);
    void UISetOnClick(std::function<void()> callback);
    void UISetName(const std::string& name);
    const std::string& UIGetName() const;

    void UIRender() override;
    void UIHandleEvent(const Event& event) override;

private:
    std::string text;
    std::string name;
    int fontSize;
    ResourceId normalTex;
    ResourceId hoverTex;
    ResourceId pressedTex;
    std::function<void()> onClick;

    enum class ButtonState {
        STATE_NORMAL,
        STATE_HOVER,
        STATE_PRESSED
    };

    ButtonState state;
};

#endif
