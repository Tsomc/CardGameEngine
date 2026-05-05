#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "module_manager.h"
#include "camera.h"
#include "sprite.h"
#include "sprite_batcher.h"
#include "resource_handle.h"

struct SDL_Renderer;

class RenderSystem : public IModule {
public:
    RenderSystem();
    ~RenderSystem() override;

    bool ModuleInit() override;
    void ModuleStart() override;
    void ModuleUpdate(float deltaTime) override;
    void ModuleRender() override;
    void ModuleShutdown() override;
    const char* ModuleGetName() const override;

    void RenderSetCamera(const Camera& camera);
    const Camera& RenderGetCamera() const;

    void RenderClear(const Color& color = Color(0, 0, 0, 255));
    void RenderDrawSprite(const Sprite& sprite);

    void RenderDrawTexture(
        ResourceId textureId,
        const Rect* src,
        const Rect* dst,
        const Color& color,
        float angle,
        const Vec2& pivot,
        FlipMode flip);

    void RenderFlush();
    void RenderPresent();

    int RenderGetWidth() const;
    int RenderGetHeight() const;
    void RenderSetLogicalSize(int width, int height);

    Vec2 RenderScreenToWorld(const Vec2& screenPos) const;
    Vec2 RenderWorldToScreen(const Vec2& worldPos) const;

    void* RenderGetSDLRenderer();

    int RenderGetDrawCallCount() const;
    int RenderGetSpriteCount() const;

private:
    SDL_Renderer* sdlRenderer;
    Camera camera;
    SpriteBatcher batcher;
    int screenWidth;
    int screenHeight;
    bool initialized;
};

#endif
