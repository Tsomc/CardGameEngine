#ifndef SDL_PLATFORM_H
#define SDL_PLATFORM_H

#include "i_platform.h"
#include <string>

struct SDL_Window;
struct SDL_Renderer;

class SdlPlatform : public IPlatform {
public:
    SdlPlatform();
    ~SdlPlatform() override;

    bool PlatformInit(
        const char* title,
        int width,
        int height,
        bool fullscreen,
        bool vsync
    ) override;

    void PlatformShutdown() override;

    void* PlatformGetWindow() override;
    void* PlatformGetRenderer() override;

    int PlatformGetWidth() const override;
    int PlatformGetHeight() const override;

    void PlatformSetLogicalSize(int width, int height) override;

    const char* PlatformGetBasePath() const override;

    void* PlatformOpenFile(const char* path) override;

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::string basePath;
    int windowWidth;
    int windowHeight;
    bool initialized;
};

#endif
