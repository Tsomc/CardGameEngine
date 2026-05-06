#include <cstdio>

#include <SDL.h>

#include "engine_all.h"

using namespace engine;

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    LOG_INFO("Main", "========================================");
    LOG_INFO("Main", "MyFirstAiGameEngine - 启动");
    LOG_INFO("Main", "========================================");

    LOG_INFO("Main", "2D 卡牌游戏引擎启动");

    EngineConfig config;
    config.windowTitle = "MyFirstAiGameEngine";
    config.windowWidth = 1280;
    config.windowHeight = 720;
    config.fullscreen = false;
    config.vsync = true;
    config.enableDebug = true;
    config.logLevel = LogLevel::LOG_LEVEL_DEBUG;

    Engine& engine = Engine::EngineGet();
    if (!engine.EngineInit(config)) {
        LOG_ERROR("Main", "引擎初始化失败");
        return 1;
    }

    ModuleManager& mm = engine.EngineGetModuleManager();

    mm.ModuleRegister<ResourceManager>();
    mm.ModuleRegister<RenderSystem>();
    mm.ModuleRegister<InputSystem>();
    mm.ModuleRegister<AudioSystem>();
    mm.ModuleRegister<ScriptSystem>();
    mm.ModuleRegister<AnimationSystem>();
    mm.ModuleRegister<CardSystem>();
    mm.ModuleRegister<SceneManager>();
    mm.ModuleRegister<UISystem>();
    mm.ModuleRegister<DebugSystem>();

    if (!mm.ModuleInitAll()) {
        LOG_ERROR("Main", "模块初始化失败，引擎关闭");
        engine.EngineShutdown();
        return 1;
    }

    mm.ModuleStartAll();

    InputSystem* input = engine.EngineGetModule<InputSystem>();
    if (input != nullptr) {
        input->InputSetupDefaultBindings();
    }

    ScriptSystem* script = engine.EngineGetModule<ScriptSystem>();
    if (script != nullptr) {
        ScriptRegisterEngineBindings(script);
    }

    CardSystem* card = engine.EngineGetModule<CardSystem>();
    if (card != nullptr) {
        LOG_INFO("Main", "卡牌系统就绪，已注册 %d 张模板", card->CardGetTemplateCount());
    }

    LOG_INFO("Main", "引擎启动完成，进入主循环");

    engine.EngineRun();

    LOG_INFO("Main", "引擎主循环结束，开始关闭");

    engine.EngineShutdown();

    LOG_INFO("Main", "引擎关闭完成");
    LOG_INFO("Main", "========================================");
    LOG_INFO("Main", "MyFirstAiGameEngine - 关闭");
    LOG_INFO("Main", "========================================");

    getchar();
    return 0;
}
