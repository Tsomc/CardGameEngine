#include "ui_image.h"
#include "engine.h"
#include "render_system.h"
using namespace engine;
void UIImage::UISetTexture(ResourceId texId)
{
    textureId = texId;
}

void UIImage::UISetColorMod(const Color& c)
{
    colorMod = c;
}

void UIImage::UISetFlip(FlipMode flip)
{
    flipMode = flip;
}

void UIImage::UIRender()
{
    if (!visible || textureId == INVALID_RESOURCE_ID) {
        return;
    }

    RenderSystem* render = Engine::EngineGet().EngineGetModule<RenderSystem>();
    if (render == nullptr) {
        return;
    }

    Sprite sprite;
    sprite.textureId = textureId;
    sprite.dstRect = rect;
    sprite.colorMod = colorMod;
    sprite.flip = flipMode;
    sprite.layer = layer;

    render->RenderDrawSprite(sprite);
}
