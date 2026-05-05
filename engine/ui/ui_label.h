#ifndef UI_LABEL_H
#define UI_LABEL_H

#include "ui_element.h"
#include "resource_handle.h"
#include <string>

class UILabel : public UIElement {
public:
    void UISetText(const std::string& text);
    void UISetFont(ResourceId fontId);
    void UISetFontSize(int size);
    void UISetColor(const Color& color);
    void UIRender() override;

private:
    std::string text;
    ResourceId fontId;
    int fontSize;
    Color color;
};

#endif
