#include "sdl_platform.h"
#include "core_log.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

SdlPlatform::SdlPlatform()
    : window(nullptr)
    , renderer(nullptr)
    , windowWidth(0)
    , windowHeight(0)
    , initialized(false)
{
}

SdlPlatform::~SdlPlatform()
{
    if (initialized) {
        PlatformShutdown();
    }
}

bool SdlPlatform::PlatformInit(
    const char* title,
    int width,
    int height,
    bool fullscreen,
    bool vsync)
{
    if (title == nullptr) {
        LOG_ERROR("Platform", "title 为空");
        return false;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        LOG_ERROR("Platform", "SDL_Init 失败: %s", SDL_GetError());
        return false;
    }

    Uint32 flags = SDL_WINDOW_SHOWN;
    if (fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        flags
    );

    if (window == nullptr) {
        LOG_ERROR("Platform", "SDL_CreateWindow 失败: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    Uint32 renderFlags = SDL_RENDERER_ACCELERATED;
    if (vsync) {
        renderFlags |= SDL_RENDERER_PRESENTVSYNC;
    }

    renderer = SDL_CreateRenderer(window, -1, renderFlags);
    if (renderer == nullptr) {
        LOG_ERROR("Platform", "SDL_CreateRenderer 失败: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        window = nullptr;
        SDL_Quit();
        return false;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderSetLogicalSize(renderer, width, height);
    SDL_StartTextInput();

    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
        LOG_WARN("Platform", "IMG_Init 部分失败: %s", IMG_GetError());
    }

    if (TTF_Init() < 0) {
        LOG_WARN("Platform", "TTF_Init 失败: %s", TTF_GetError());
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
        LOG_WARN("Platform", "Mix_OpenAudio 失败: %s", Mix_GetError());
    }

    Mix_AllocateChannels(16);

    char* base = SDL_GetBasePath();
    if (base != nullptr) {
        basePath = base;
        SDL_free(base);
    }

    windowWidth = width;
    windowHeight = height;
    initialized = true;

    LOG_INFO("Platform", "初始化成功: %dx%d %s %s",
             width, height,
             fullscreen ? "全屏" : "窗口",
             vsync ? "垂直同步" : "无垂直同步");

    return true;
}

void SdlPlatform::PlatformShutdown()
{
    if (!initialized) {
        return;
    }

    LOG_INFO("Platform", "开始关闭");

    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_StopTextInput();

    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
    initialized = false;

    LOG_INFO("Platform", "关闭完成");
}

void* SdlPlatform::PlatformGetWindow()
{
    return (void*)window;
}

void* SdlPlatform::PlatformGetRenderer()
{
    return (void*)renderer;
}

int SdlPlatform::PlatformGetWidth() const
{
    return windowWidth;
}

int SdlPlatform::PlatformGetHeight() const
{
    return windowHeight;
}

void SdlPlatform::PlatformSetLogicalSize(int width, int height)
{
    if (renderer != nullptr) {
        SDL_RenderSetLogicalSize(renderer, width, height);
        windowWidth = width;
        windowHeight = height;
    }
}

const char* SdlPlatform::PlatformGetBasePath() const
{
    return basePath.c_str();
}

void* SdlPlatform::PlatformOpenFile(const char* path)
{
    if (path == nullptr) {
        LOG_ERROR("Platform", "PlatformOpenFile: path 为空");
        return nullptr;
    }

    std::string fullPath = basePath + path;
    SDL_RWops* rw = SDL_RWFromFile(fullPath.c_str(), "rb");
    if (rw == nullptr) {
        LOG_WARN("Platform", "无法打开文件: %s (%s)", fullPath.c_str(), SDL_GetError());
        return nullptr;
    }

    return (void*)rw;
}
