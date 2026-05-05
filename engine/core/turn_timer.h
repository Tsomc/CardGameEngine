#ifndef TURN_TIMER_H
#define TURN_TIMER_H

#include "event_bus.h"
#include <cstdint>

using PlayerId = uint8_t;
constexpr PlayerId INVALID_PLAYER_ID = 0xFF;

struct TurnTimeoutEvent : Event {
    PlayerId playerId;
};

class TurnTimer {
public:
    TurnTimer();

    void TurnTimerStart(float durationSeconds, PlayerId playerId);
    void TurnTimerPause();
    void TurnTimerResume();
    void TurnTimerStop();

    float TurnTimerGetRemaining() const;
    bool TurnTimerIsRunning() const;

    void TurnTimerUpdate(float deltaTime);

private:
    float remaining;
    float duration;
    PlayerId currentPlayer;
    bool running;
    bool paused;
};

#endif
