#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include "module_manager.h"
#include "event_bus.h"
#include "input_action.h"
#include "math_types.h"
#include <array>
#include <unordered_map>
#include <string>

struct InputActionEvent : Event {
    InputAction action;
    InputActionState state;
    Vec2 position;
};

struct MouseMoveEvent : Event {
    Vec2 position;
    Vec2 delta;
};

struct HoverBeginEvent : Event {
    Vec2 position;
    uint32_t cardId;
    uint32_t uiElementId;
};

struct HoverEndEvent : Event {
    uint32_t cardId;
    uint32_t uiElementId;
};

struct WindowResizeEvent : Event {
    int newWidth;
    int newHeight;
};

struct WindowCloseEvent : Event {};

struct EngineQuitEvent : Event {};

constexpr int MAX_ACTION_COUNT = 1024;

class InputSystem : public IModule {
public:
    InputSystem();
    ~InputSystem() override;

    bool ModuleInit() override;
    void ModuleStart() override;
    void ModuleUpdate(float deltaTime) override;
    void ModuleShutdown() override;
    const char* ModuleGetName() const override;

    void InputUpdate();

    bool InputIsActionPressed(InputAction action) const;
    bool InputIsActionHeld(InputAction action) const;
    bool InputIsActionReleased(InputAction action) const;

    Vec2 InputGetMousePosition() const;
    Vec2 InputGetWorldPosition() const;

    void InputBindKey(int sdlKeyCode, InputAction action);
    void InputBindMouse(int sdlMouseButton, InputAction action);

    InputAction InputRegisterAction(const std::string& name);
    const std::string& InputGetActionName(InputAction action) const;

    void InputSetEnabled(bool enabled);
    bool InputIsEnabled() const;

    void InputSetupDefaultBindings();

private:
    void FireActionEvent(InputAction action, InputActionState state);

    std::array<bool, MAX_ACTION_COUNT> prevStates;
    std::array<bool, MAX_ACTION_COUNT> currStates;
    std::array<InputActionState, MAX_ACTION_COUNT> currActions;

    std::unordered_map<int, InputAction> keyBindings;
    std::unordered_map<int, InputAction> mouseBindings;
    std::unordered_map<int, std::string> actionNames;

    Vec2 mousePosition;
    int nextActionId;
    bool enabled;
};

#endif
