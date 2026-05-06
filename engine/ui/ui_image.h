#ifndef UI_IMAGE_H
#define UI_IMAGE_H

#include "ui_element.h"
#include "resource_handle.h"
#include "sprite.h"


namespace engine {

class UIImage : public UIElement {
public:
    void UISetTexture(ResourceId texId);
    void UISetColorMod(const Color& color);
    void UISetFlip(FlipMode flip);
    void UIRender() override;

private:
    ResourceId textureId;
    Color colorMod;
    FlipMode flipMode;
};


} /* namespace engine */
#endif
