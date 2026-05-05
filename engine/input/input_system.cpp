#include "input_system.h"
#include "core_log.h"
#include "engine.h"
#include "render_system.h"
#include <SDL.h>

InputSystem::InputSystem()
    : mousePosition(0.0f, 0.0f)
    , enabled(true)
{
    prevStates.fill(false);
    currStates.fill(false);
}

InputSystem::~InputSystem()
{
}

bool InputSystem::ModuleInit()
{
    LOG_INFO("Input", "输入系统初始化");
    return true;
}

void InputSystem::ModuleStart()
{
}

void InputSystem::ModuleUpdate(float deltaTime)
{
    (void)deltaTime;
    InputUpdate();
}

void InputSystem::ModuleShutdown()
{
    LOG_INFO("Input", "输入系统关闭");
}

const char* InputSystem::ModuleGetName() const
{
    return "InputSystem";
}

void InputSystem::InputUpdate()
{
    if (!enabled) {
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        return;
    }

    prevStates = currStates;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: {
                EngineQuitEvent quitEvent;
                quitEvent.eventTimestamp = 0.0f;
                Engine::EngineGet().EngineGetEventBus().EventEmit(quitEvent);
                Engine::EngineGet().EngineQuit();
                break;
            }

            case SDL_KEYDOWN: {
                auto it = keyBindings.find(event.key.keysym.sym);
                if (it != keyBindings.end()) {
                    currStates[it->second] = true;
                    FireActionEvent(it->second, InputActionState::INPUT_STATE_PRESSED);
                }
                break;
            }

            case SDL_KEYUP: {
                auto it = keyBindings.find(event.key.keysym.sym);
                if (it != keyBindings.end()) {
                    currStates[it->second] = false;
                    FireActionEvent(it->second, InputActionState::INPUT_STATE_RELEASED);
                }
                break;
            }

            case SDL_MOUSEBUTTONDOWN: {
                int button = event.button.button;
                auto it = mouseBindings.find(button);
                if (it != mouseBindings.end()) {
                    currStates[it->second] = true;
                    FireActionEvent(it->second, InputActionState::INPUT_STATE_PRESSED);
                }
                break;
            }

            case SDL_MOUSEBUTTONUP: {
                int button = event.button.button;
                auto it = mouseBindings.find(button);
                if (it != mouseBindings.end()) {
                    currStates[it->second] = false;
                    FireActionEvent(it->second, InputActionState::INPUT_STATE_RELEASED);
                }
                break;
            }

            case SDL_MOUSEMOTION: {
                Vec2 oldPos = mousePosition;
                mousePosition.x = (float)event.motion.x;
                mousePosition.y = (float)event.motion.y;

                MouseMoveEvent mmEvent;
                mmEvent.position = mousePosition;
                mmEvent.delta = Vec2(
                    mousePosition.x - oldPos.x,
                    mousePosition.y - oldPos.y
                );
                Engine::EngineGet().EngineGetEventBus().EventEmit(mmEvent);
                break;
            }

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED ||
                    event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    WindowResizeEvent wrEvent;
                    wrEvent.newWidth = event.window.data1;
                    wrEvent.newHeight = event.window.data2;
                    Engine::EngineGet().EngineGetEventBus().EventEmit(wrEvent);
                }
                break;
        }
    }

    for (size_t i = 0; i < currStates.size(); ++i) {
        if (currStates[i] && prevStates[i]) {
            FireActionEvent((InputAction)i, InputActionState::INPUT_STATE_HELD);
        }
    }
}

bool InputSystem::InputIsActionPressed(InputAction action) const
{
    return currActions[(int)action] == InputActionState::INPUT_STATE_PRESSED;
}

bool InputSystem::InputIsActionHeld(InputAction action) const
{
    return currActions[(int)action] == InputActionState::INPUT_STATE_HELD;
}

bool InputSystem::InputIsActionReleased(InputAction action) const
{
    return currActions[(int)action] == InputActionState::INPUT_STATE_RELEASED;
}

Vec2 InputSystem::InputGetMousePosition() const
{
    return mousePosition;
}

Vec2 InputSystem::InputGetWorldPosition() const
{
    RenderSystem* render = Engine::EngineGet().EngineGetModule<RenderSystem>();
    if (render != nullptr) {
        return render->RenderScreenToWorld(mousePosition);
    }
    return mousePosition;
}

void InputSystem::InputBindKey(int sdlKeyCode, InputAction action)
{
    if (action < keyBindings.size()) {
        keyBindings[sdlKeyCode] = action;
    }
}

void InputSystem::InputBindMouse(int sdlMouseButton, InputAction action)
{
    if (action < mouseBindings.size()) {
        mouseBindings[sdlMouseButton] = action;
    }
}

InputAction InputSystem::InputRegisterAction(const std::string& name)
{
    if (nextActionId >= MAX_ACTION_COUNT) {
        LOG_ERROR("Input", "动作注册已满");
        return ACTION_NONE;
    }
    InputAction action = (InputAction)nextActionId++;
    actionNames[action] = name;
    return action;
}

const std::string& InputSystem::InputGetActionName(InputAction action) const
{
    static std::string empty;
    auto it = actionNames.find(action);
    if (it != actionNames.end()) {
        return it->second;
    }
    return empty;
}

void InputSystem::InputSetEnabled(bool e)
{
    enabled = e;
}

bool InputSystem::InputIsEnabled() const
{
    return enabled;
}

void InputSystem::InputSetupDefaultBindings()
{
    keyBindings[SDLK_RETURN] = ACTION_SELECT;
    keyBindings[SDLK_ESCAPE] = ACTION_CANCEL;
    keyBindings[SDLK_ESCAPE] = ACTION_MENU;
    keyBindings[SDLK_SPACE] = ACTION_SKIP;
    keyBindings[SDLK_F1] = ACTION_DEBUG_TOGGLE;
    keyBindings[SDLK_BACKQUOTE] = ACTION_DEBUG_CONSOLE;
    keyBindings[SDLK_F10] = ACTION_DEBUG_STEP;

    mouseBindings[SDL_BUTTON_LEFT] = ACTION_SELECT;
    mouseBindings[SDL_BUTTON_RIGHT] = ACTION_CANCEL;
}

void InputSystem::FireActionEvent(InputAction action, InputActionState state)
{
    currActions[(int)action] = state;

    InputActionEvent event;
    event.action = action;
    event.state = state;
    event.position = mousePosition;
    Engine::EngineGet().EngineGetEventBus().EventEmit(event);
}
