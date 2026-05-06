#ifndef ENGINE_CONFIG_H
#define ENGINE_CONFIG_H

#include "core_log.h"


namespace engine {

struct EngineConfig {
    const char* windowTitle;
    int windowWidth;
    int windowHeight;
    bool fullscreen;
    bool vsync;
    const char* assetRootPath;
    bool enableDebug;
    LogLevel logLevel;
    int maxFPS;
    float defaultTimeScale;

    EngineConfig()
        : windowTitle("MyFirstAiGameEngine")
        , windowWidth(1280)
        , windowHeight(720)
        , fullscreen(false)
        , vsync(true)
        , assetRootPath("assets/")
        , enableDebug(true)
        , logLevel(LogLevel::LOG_LEVEL_DEBUG)
        , maxFPS(60)
        , defaultTimeScale(1.0f)
    {
    }
};


} /* namespace engine */
#endif
