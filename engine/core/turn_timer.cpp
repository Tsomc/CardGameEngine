#include "turn_timer.h"
#include "engine.h"

TurnTimer::TurnTimer()
    : remaining(0.0f)
    , duration(0.0f)
    , currentPlayer(INVALID_PLAYER_ID)
    , running(false)
    , paused(false)
{
}

void TurnTimer::TurnTimerStart(float durationSeconds, PlayerId playerId)
{
    duration = durationSeconds;
    remaining = durationSeconds;
    currentPlayer = playerId;
    running = true;
    paused = false;
}

void TurnTimer::TurnTimerPause()
{
    paused = true;
}

void TurnTimer::TurnTimerResume()
{
    paused = false;
}

void TurnTimer::TurnTimerStop()
{
    running = false;
    paused = false;
    remaining = 0.0f;
}

float TurnTimer::TurnTimerGetRemaining() const
{
    return remaining;
}

bool TurnTimer::TurnTimerIsRunning() const
{
    return running && !paused;
}

void TurnTimer::TurnTimerUpdate(float deltaTime)
{
    if (!running || paused) {
        return;
    }

    remaining -= deltaTime;
    if (remaining <= 0.0f) {
        remaining = 0.0f;
        running = false;

        TurnTimeoutEvent event;
        event.playerId = currentPlayer;
        Engine::EngineGet().EngineGetEventBus().EventEmit(event);
    }
}
