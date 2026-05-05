#ifndef SCENE_H
#define SCENE_H

#include "event_bus.h"
#include <string>
#include <vector>

class IScene {
public:
    virtual ~IScene() = default;

    virtual void SceneOnEnter() = 0;
    virtual void SceneOnExit() = 0;
    virtual void SceneOnPause() = 0;
    virtual void SceneOnResume() = 0;
    virtual void SceneUpdate(float deltaTime) = 0;
    virtual void SceneRender() = 0;
    virtual void SceneHandleEvent(const Event& event) = 0;
    virtual const std::string& SceneGetName() const = 0;

    std::vector<EventHandle> eventHandles;
};

#endif
