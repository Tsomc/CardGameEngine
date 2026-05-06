#include "ui_label.h"
using namespace engine;
void UILabel::UISetText(const std::string& t)
{
    text = t;
}

void UILabel::UISetFont(ResourceId fontId)
{
    this->fontId = fontId;
}

void UILabel::UISetFontSize(int size)
{
    fontSize = size;
}

void UILabel::UISetColor(const Color& c)
{
    color = c;
}

void UILabel::UIRender()
{
    if (!visible) {
        return;
    }
}
