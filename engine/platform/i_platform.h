#ifndef I_PLATFORM_H
#define I_PLATFORM_H


namespace engine {

class IPlatform {
public:
    virtual ~IPlatform() = default;

    virtual bool PlatformInit(
        const char* title,
        int width,
        int height,
        bool fullscreen,
        bool vsync
    ) = 0;

    virtual void PlatformShutdown() = 0;

    virtual void* PlatformGetWindow() = 0;

    virtual void* PlatformGetRenderer() = 0;

    virtual int PlatformGetWidth() const = 0;
    virtual int PlatformGetHeight() const = 0;

    virtual void PlatformSetLogicalSize(int width, int height) = 0;

    virtual const char* PlatformGetBasePath() const = 0;

    virtual void* PlatformOpenFile(const char* path) = 0;
};


} /* namespace engine */
#endif
