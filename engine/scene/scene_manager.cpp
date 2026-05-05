#include "scene_manager.h"
#include "core_log.h"
#include "engine.h"
#include "event_bus.h"

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
    while (!sceneStack.empty()) {
        ScenePop();
    }
}

bool SceneManager::ModuleInit()
{
    LOG_INFO("Scene", "场景管理器初始化");
    return true;
}

void SceneManager::ModuleStart()
{
}

void SceneManager::ModuleUpdate(float deltaTime)
{
    if (!sceneStack.empty()) {
        sceneStack.top()->SceneUpdate(deltaTime);
    }
}

void SceneManager::ModuleRender()
{
    if (!sceneStack.empty()) {
        sceneStack.top()->SceneRender();
    }
}

void SceneManager::ModuleShutdown()
{
    while (!sceneStack.empty()) {
        sceneStack.top()->SceneOnExit();
        CancelSceneSubscriptions(sceneStack.top().get());
        sceneStack.pop();
    }
    LOG_INFO("Scene", "场景管理器关闭");
}

const char* SceneManager::ModuleGetName() const
{
    return "SceneManager";
}

void SceneManager::ScenePop()
{
    if (sceneStack.empty()) {
        return;
    }

    std::shared_ptr<IScene> current = sceneStack.top();

    current->SceneOnExit();

    CancelSceneSubscriptions(current.get());

    Engine::EngineGet().EngineGetEventBus().EventClearDeferred();

    current.reset();
    sceneStack.pop();

    if (!sceneStack.empty()) {
        sceneStack.top()->SceneOnResume();
    }
}

void SceneManager::ScenePopInternal()
{
    if (sceneStack.empty()) {
        return;
    }

    std::shared_ptr<IScene> current = sceneStack.top();

    CancelSceneSubscriptions(current.get());

    current.reset();
    sceneStack.pop();
}

IScene* SceneManager::SceneGetCurrent() const
{
    if (sceneStack.empty()) {
        return nullptr;
    }
    return sceneStack.top().get();
}

bool SceneManager::SceneIsEmpty() const
{
    return sceneStack.empty();
}

int SceneManager::SceneGetStackSize() const
{
    return (int)sceneStack.size();
}

void SceneManager::CancelSceneSubscriptions(IScene* scene)
{
    if (scene == nullptr) {
        return;
    }

    for (EventHandle handle : scene->eventHandles) {
        Engine::EngineGet().EngineGetEventBus().EventUnsubscribeAny(handle);
    }
    scene->eventHandles.clear();
}
