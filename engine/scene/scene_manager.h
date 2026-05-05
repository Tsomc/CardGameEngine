#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "module_manager.h"
#include "scene.h"
#include <memory>
#include <stack>

struct UISceneLoadedEvent : Event {
    std::string sceneName;
};

class SceneManager : public IModule {
public:
    SceneManager();
    ~SceneManager() override;

    bool ModuleInit() override;
    void ModuleStart() override;
    void ModuleUpdate(float deltaTime) override;
    void ModuleRender() override;
    void ModuleShutdown() override;
    const char* ModuleGetName() const override;

    template<typename T, typename... Args>
    void ScenePush(Args&&... args)
    {
        if (!sceneStack.empty()) {
            sceneStack.top()->SceneOnPause();
        }

        auto scene = std::make_shared<T>(std::forward<Args>(args)...);
        sceneStack.push(scene);
        scene->SceneOnEnter();
    }

    void ScenePop();

    template<typename T, typename... Args>
    void SceneReplace(Args&&... args)
    {
        ScenePop();
        ScenePush<T>(std::forward<Args>(args)...);
    }

    IScene* SceneGetCurrent() const;
    bool SceneIsEmpty() const;
    int SceneGetStackSize() const;

private:
    void CancelSceneSubscriptions(IScene* scene);
    void ScenePopInternal();

    std::stack<std::shared_ptr<IScene>> sceneStack;
};

#endif
