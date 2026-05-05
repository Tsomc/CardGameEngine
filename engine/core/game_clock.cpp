#include "game_clock.h"
#include <algorithm>
#include <SDL.h>

GameClock::GameClock()
    : frequency(0)
    , lastCounter(0)
    , deltaTime(0.0f)
    , rawDelta(0.0f)
    , totalTime(0.0f)
    , timeScale(1.0f)
    , initialized(false)
{
}

void GameClock::GameClockInit()
{
    frequency = (double)SDL_GetPerformanceFrequency();
    lastCounter = SDL_GetPerformanceCounter();
    totalTime = 0.0f;
    deltaTime = 0.0f;
    rawDelta = 0.0f;
    timeScale = 1.0f;
    initialized = true;
}

void GameClock::GameClockTick()
{
    if (!initialized) {
        return;
    }

    Uint64 currentCounter = SDL_GetPerformanceCounter();
    double elapsed = (double)(currentCounter - lastCounter) / frequency;

    if (elapsed > 0.1) {
        elapsed = 1.0 / 30.0;
    }

    lastCounter = currentCounter;

    rawDelta = (float)elapsed;
    deltaTime = rawDelta * timeScale;
    totalTime += rawDelta;
}

float GameClock::GameClockGetDelta() const
{
    return deltaTime;
}

float GameClock::GameClockGetTotalTime() const
{
    return totalTime;
}

void GameClock::GameClockSetTimeScale(float scale)
{
    timeScale = std::max(0.0f, scale);
}

float GameClock::GameClockGetTimeScale() const
{
    return timeScale;
}

float GameClock::GameClockGetRawDelta() const
{
    return rawDelta;
}
