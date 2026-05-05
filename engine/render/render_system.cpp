#include "render_system.h"
#include "core_log.h"
#include "engine.h"
#include "i_platform.h"
#include <SDL.h>

RenderSystem::RenderSystem()
    : sdlRenderer(nullptr)
    , screenWidth(1280)
    , screenHeight(720)
    , initialized(false)
{
}

RenderSystem::~RenderSystem()
{
}

bool RenderSystem::ModuleInit()
{
    IPlatform& plat = Engine::EngineGet().EngineGetPlatform();
    sdlRenderer = (SDL_Renderer*)plat.PlatformGetRenderer();

    if (sdlRenderer == nullptr) {
        LOG_ERROR("Render", "初始化失败：渲染器为空");
        return false;
    }

    screenWidth = plat.PlatformGetWidth();
    screenHeight = plat.PlatformGetHeight();

    initialized = true;
    LOG_INFO("Render", "渲染系统初始化: %dx%d", screenWidth, screenHeight);
    return true;
}

void RenderSystem::ModuleStart()
{
}

void RenderSystem::ModuleUpdate(float deltaTime)
{
    (void)deltaTime;
}

void RenderSystem::ModuleRender()
{
    if (!initialized || sdlRenderer == nullptr) {
        return;
    }

    RenderFlush();
    RenderPresent();
}

void RenderSystem::ModuleShutdown()
{
    sdlRenderer = nullptr;
    initialized = false;
    LOG_INFO("Render", "渲染系统关闭");
}

const char* RenderSystem::ModuleGetName() const
{
    return "RenderSystem";
}

void RenderSystem::RenderSetCamera(const Camera& cam)
{
    camera = cam;
}

const Camera& RenderSystem::RenderGetCamera() const
{
    return camera;
}

void RenderSystem::RenderClear(const Color& color)
{
    if (sdlRenderer == nullptr) {
        return;
    }
    SDL_SetRenderDrawColor(sdlRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(sdlRenderer);
}

void RenderSystem::RenderDrawSprite(const Sprite& sprite)
{
    if (!initialized) {
        return;
    }
    batcher.RenderDrawSprite(sprite);
}

void RenderSystem::RenderDrawTexture(
    ResourceId textureId,
    const Rect* src,
    const Rect* dst,
    const Color& color,
    float angle,
    const Vec2& pivot,
    FlipMode flip)
{
    if (!initialized || sdlRenderer == nullptr || textureId == INVALID_RESOURCE_ID) {
        return;
    }

    Sprite sprite;
    sprite.textureId = textureId;
    if (src != nullptr) {
        sprite.srcRect = *src;
    }
    if (dst != nullptr) {
        sprite.dstRect = *dst;
    } else {
        sprite.dstRect = Rect(0, 0, 0, 0);
    }
    sprite.colorMod = color;
    sprite.angle = angle;
    sprite.pivot = pivot;
    sprite.flip = flip;

    batcher.RenderDrawSprite(sprite);
}

void RenderSystem::RenderFlush()
{
    if (sdlRenderer == nullptr) {
        return;
    }
    batcher.RenderFlush((void*)sdlRenderer);
}

void RenderSystem::RenderPresent()
{
    if (sdlRenderer == nullptr) {
        return;
    }
    SDL_RenderPresent(sdlRenderer);
}

int RenderSystem::RenderGetWidth() const
{
    return screenWidth;
}

int RenderSystem::RenderGetHeight() const
{
    return screenHeight;
}

void RenderSystem::RenderSetLogicalSize(int width, int height)
{
    Engine::EngineGet().EngineGetPlatform().PlatformSetLogicalSize(width, height);
    screenWidth = width;
    screenHeight = height;
}

Vec2 RenderSystem::RenderScreenToWorld(const Vec2& screenPos) const
{
    return camera.CameraScreenToWorld(screenPos, screenWidth, screenHeight);
}

Vec2 RenderSystem::RenderWorldToScreen(const Vec2& worldPos) const
{
    return camera.CameraWorldToScreen(worldPos, screenWidth, screenHeight);
}

void* RenderSystem::RenderGetSDLRenderer()
{
    return (void*)sdlRenderer;
}

int RenderSystem::RenderGetDrawCallCount() const
{
    return batcher.RenderGetDrawCallCount();
}

int RenderSystem::RenderGetSpriteCount() const
{
    return batcher.RenderGetSpriteCount();
}
