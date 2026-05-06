#ifndef GAME_CLOCK_H
#define GAME_CLOCK_H

#include <cstdint>


namespace engine {

class GameClock {
public:
    GameClock();

    void GameClockInit();
    void GameClockTick();

    float GameClockGetDelta() const;
    float GameClockGetTotalTime() const;

    void GameClockSetTimeScale(float scale);
    float GameClockGetTimeScale() const;

    float GameClockGetRawDelta() const;

private:
    double frequency;
    uint64_t lastCounter;
    float deltaTime;
    float rawDelta;
    float totalTime;
    float timeScale;
    bool initialized;
};


} /* namespace engine */
#endif
