#ifndef ENGINE_H
#define ENGINE_H

#include "engine_config.h"
#include "event_bus.h"
#include "module_manager.h"
#include "game_clock.h"
#include "i_platform.h"

#include <memory>
#include <cstdint>
#include <random>


namespace engine {

class Engine {
public:
    static Engine& EngineGet();

    bool EngineInit(const EngineConfig& config);
    void EngineRun();
    void EngineQuit();
    void EngineShutdown();

    template<typename T>
    T* EngineGetModule()
    {
        if (moduleManager == nullptr) {
            return nullptr;
        }
        return moduleManager->ModuleGet<T>();
    }

    EventBus& EngineGetEventBus();
    IPlatform& EngineGetPlatform();
    GameClock& EngineGetGameClock();
    ModuleManager& EngineGetModuleManager();

    void EngineSetRandomSeed(uint64_t seed);
    uint64_t EngineGetRandomSeed() const;

    std::mt19937_64& EngineGetRNG();

private:
    Engine();
    ~Engine();
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    bool LoadConfigFromJson(const char* jsonPath);

    std::unique_ptr<IPlatform> platform;
    std::unique_ptr<ModuleManager> moduleManager;
    EventBus eventBus;
    GameClock gameClock;
    EngineConfig config;

    bool quit;
    bool initialized;

    std::mt19937_64 rng;
    uint64_t randomSeed;
};


} /* namespace engine */
#endif
