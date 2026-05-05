#include "ui_button.h"
#include "engine.h"
#include "render_system.h"
#include "sprite.h"
#include <SDL.h>

UIButton::UIButton()
    : fontSize(16)
    , normalTex(INVALID_RESOURCE_ID)
    , hoverTex(INVALID_RESOURCE_ID)
    , pressedTex(INVALID_RESOURCE_ID)
    , state(ButtonState::STATE_NORMAL)
{
}

void UIButton::UISetText(const std::string& t)
{
    text = t;
}

void UIButton::UISetFontSize(int size)
{
    fontSize = size;
}

void UIButton::UISetNormalTexture(ResourceId texId)
{
    normalTex = texId;
}

void UIButton::UISetHoverTexture(ResourceId texId)
{
    hoverTex = texId;
}

void UIButton::UISetPressedTexture(ResourceId texId)
{
    pressedTex = texId;
}

void UIButton::UISetOnClick(std::function<void()> callback)
{
    onClick = callback;
}

void UIButton::UISetName(const std::string& n)
{
    name = n;
}

const std::string& UIButton::UIGetName() const
{
    return name;
}

void UIButton::UIRender()
{
    if (!visible) {
        return;
    }

    ResourceId texId = INVALID_RESOURCE_ID;
    switch (state) {
        case ButtonState::STATE_NORMAL: texId = normalTex; break;
        case ButtonState::STATE_HOVER:  texId = hoverTex; break;
        case ButtonState::STATE_PRESSED: texId = pressedTex; break;
    }

    if (texId != INVALID_RESOURCE_ID) {
        RenderSystem* render = Engine::EngineGet().EngineGetModule<RenderSystem>();
        if (render != nullptr) {
            Sprite sprite;
            sprite.textureId = texId;
            sprite.dstRect = rect;
            render->RenderDrawSprite(sprite);
        }
    }
}

void UIButton::UIHandleEvent(const Event& event)
{
    (void)event;
}
