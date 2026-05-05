#include "engine.h"
#include "core_log.h"
#include "sdl_platform.h"
#include <SDL.h>
#include <nlohmann/json.hpp>
#include <fstream>

Engine::Engine()
    : quit(false)
    , initialized(false)
    , rng(std::random_device{}())
    , randomSeed(0)
{
}

Engine::~Engine()
{
    if (initialized) {
        EngineShutdown();
    }
}

Engine& Engine::EngineGet()
{
    static Engine instance;
    return instance;
}

bool Engine::EngineInit(const EngineConfig& config)
{
    if (initialized) {
        LOG_WARN("Core", "引擎已经初始化");
        return true;
    }

    this->config = config;

    LogSetLevel(config.logLevel);
    LOG_INFO("Core", "引擎启动...");

    platform = std::make_unique<SdlPlatform>();
    if (!platform->PlatformInit(
            config.windowTitle,
            config.windowWidth,
            config.windowHeight,
            config.fullscreen,
            config.vsync)) {
        LOG_ERROR("Core", "平台初始化失败");
        platform.reset();
        return false;
    }

    gameClock.GameClockInit();

    moduleManager = std::make_unique<ModuleManager>();

    LoadConfigFromJson("engine.json");

    if (config.enableDebug) {
        LOG_INFO("Core", "调试模式已启用");
    }

    randomSeed = (uint64_t)SDL_GetPerformanceCounter();
    rng.seed(randomSeed);

    initialized = true;
    LOG_INFO("Core", "引擎初始化完成");

    return true;
}

bool Engine::LoadConfigFromJson(const char* jsonPath)
{
    std::string fullPath;
    if (platform != nullptr) {
        fullPath = std::string(platform->PlatformGetBasePath()) + jsonPath;
    } else {
        fullPath = jsonPath;
    }

    std::ifstream file(fullPath);
    if (!file.is_open()) {
        LOG_INFO("Core", "未找到引擎配置文件 %s，使用默认配置", fullPath.c_str());
        return false;
    }

    static std::string titleBuffer;
    try {
        nlohmann::json j;
        file >> j;

        if (j.contains("window")) {
            auto& w = j["window"];
            if (w.contains("title") && w["title"].is_string()) {
                titleBuffer = w["title"].get<std::string>();
                config.windowTitle = titleBuffer.c_str();
            }
            if (w.contains("width") && w["width"].is_number()) {
                config.windowWidth = w["width"].get<int>();
            }
            if (w.contains("height") && w["height"].is_number()) {
                config.windowHeight = w["height"].get<int>();
            }
            if (w.contains("fullscreen") && w["fullscreen"].is_boolean()) {
                config.fullscreen = w["fullscreen"].get<bool>();
            }
            if (w.contains("vsync") && w["vsync"].is_boolean()) {
                config.vsync = w["vsync"].get<bool>();
            }
        }

        if (j.contains("maxFPS") && j["maxFPS"].is_number()) {
            config.maxFPS = j["maxFPS"].get<int>();
        }
        if (j.contains("defaultTimeScale") && j["defaultTimeScale"].is_number()) {
            config.defaultTimeScale = j["defaultTimeScale"].get<float>();
        }
        if (j.contains("enableDebug") && j["enableDebug"].is_boolean()) {
            config.enableDebug = j["enableDebug"].get<bool>();
        }
        if (j.contains("logLevel") && j["logLevel"].is_number()) {
            config.logLevel = (LogLevel)j["logLevel"].get<int>();
        }

        LOG_INFO("Core", "引擎配置已从 %s 加载", fullPath.c_str());
        return true;
    } catch (const std::exception& e) {
        LOG_WARN("Core", "解析引擎配置文件失败: %s，使用默认配置", e.what());
        return false;
    }
}

void Engine::EngineRun()
{
    if (!initialized) {
        LOG_ERROR("Core", "引擎未初始化，无法运行");
        return;
    }

    LOG_INFO("Core", "引擎主循环开始");
    gameClock.GameClockInit();

    const float targetFrameTime = config.maxFPS > 0 ? 1.0f / config.maxFPS : 0.0f;

    while (!quit) {
        float dt = gameClock.GameClockGetDelta();

        moduleManager->ModuleUpdateAll(dt);

        moduleManager->ModuleRenderAll();

        eventBus.EventFlushDeferred();

        if (targetFrameTime > 0.0f) {
            float frameTime = gameClock.GameClockGetRawDelta();
            if (frameTime < targetFrameTime) {
                SDL_Delay((Uint32)((targetFrameTime - frameTime) * 1000.0f));
            }
        }

        gameClock.GameClockTick();
    }

    LOG_INFO("Core", "引擎主循环结束");
}

void Engine::EngineQuit()
{
    quit = true;
    LOG_INFO("Core", "收到退出请求");
}

void Engine::EngineShutdown()
{
    if (!initialized) {
        return;
    }

    LOG_INFO("Core", "引擎关闭开始");

    if (moduleManager) {
        moduleManager->ModuleShutdownAll();
        moduleManager.reset();
    }

    if (platform) {
        platform->PlatformShutdown();
        platform.reset();
    }

    eventBus.ClearAll();

    initialized = false;
    LOG_INFO("Core", "引擎关闭完成");
}

EventBus& Engine::EngineGetEventBus()
{
    return eventBus;
}

IPlatform& Engine::EngineGetPlatform()
{
    return *platform;
}

GameClock& Engine::EngineGetGameClock()
{
    return gameClock;
}

ModuleManager& Engine::EngineGetModuleManager()
{
    return *moduleManager;
}

void Engine::EngineSetRandomSeed(uint64_t seed)
{
    randomSeed = seed;
    rng.seed(seed);
}

uint64_t Engine::EngineGetRandomSeed() const
{
    return randomSeed;
}

std::mt19937_64& Engine::EngineGetRNG()
{
    return rng;
}
