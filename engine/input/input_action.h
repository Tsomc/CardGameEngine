#ifndef INPUT_ACTION_H
#define INPUT_ACTION_H

#include <cstdint>

enum InputAction : uint16_t {
    ACTION_NONE           = 0,
    ACTION_SELECT         = 1,
    ACTION_CANCEL         = 2,
    ACTION_MENU           = 3,
    ACTION_SKIP           = 4,

    ACTION_DEBUG_TOGGLE   = 500,
    ACTION_DEBUG_CONSOLE  = 501,
    ACTION_DEBUG_STEP     = 502,

    ACTION_ENGINE_MAX     = 503
};

enum class InputActionState : uint8_t {
    INPUT_STATE_PRESSED  = 0,
    INPUT_STATE_HELD     = 1,
    INPUT_STATE_RELEASED = 2
};

#endif
