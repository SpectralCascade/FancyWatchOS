#include <Arduino.h>
#include <math.h>

#include "time.h"
#include "utils.h"

///
/// Clock definitions
///

Clock::Clock(uint32_t startTimeMS)
{
    initialTime = startTimeMS;
}

bool Clock::Update(float deltaTimeSeconds)
{
    if (!paused)
    {
        previousTime = time;
        overflow = 0;
        if (scale < 0.0f)
        {
            uint32_t changeInTime = (uint32_t)round(((-scale) * deltaTimeSeconds) * 1000.0f);
            if (wrapValue != 0)
            {
                time = (uint32_t)Wrap(time, -(int)changeInTime, 0, wrapValue);
                if (time > previousTime)
                {
                    overflow = (int)previousTime + (int)wrapValue;
                    return true;
                }
            }
            else
            {
                /// Clamping is applied to reversed timers
                if (changeInTime > time)
                {
                    time = 0;
                }
                else
                {
                    time -= changeInTime;
                }
            }
        }
        else
        {
            if (wrapValue != 0)
            {
                time = (uint32_t)Wrap(time, (int)round((scale * deltaTimeSeconds) * 1000.0f), 0, wrapValue);
                if (time < previousTime)
                {
                    overflow = (int)wrapValue - (int)previousTime;
                    return true;
                }
            }
            else
            {
                time += (uint32_t)round((scale * deltaTimeSeconds) * 1000.0f);
            }
        }
    }
    return false;
}

void Clock::SetPaused(bool pause)
{
    paused = pause;
}

bool Clock::IsPaused()
{
    return paused;
}

void Clock::Scale(float scaleFactor)
{
    scale = scaleFactor;
}

float Clock::GetScaleFactor()
{
    return scale;
}

void Clock::StepFrames(int frames, float framePeriod)
{
    float deltaTime = (float)frames * framePeriod * 1000.0f;
    previousTime = time;
    if (deltaTime < 0)
    {
        if (wrapValue != 0)
        {
            time = (uint32_t)Wrap(time, (int)round(deltaTime), 0, wrapValue);
            if (time > previousTime)
            {
                overflow = (int)previousTime + (int)wrapValue;
            }
        }
        else
        {
            if ((uint32_t)round(-deltaTime) > time)
            {
                time = 0;
            }
            else
            {
                time -= (uint32_t)round(-deltaTime);
            }
        }
    }
    else
    {
        if (wrapValue != 0)
        {
            time = Wrap(time, (int)round(deltaTime), 0, wrapValue);
            if (time < previousTime)
            {
                overflow = (int)wrapValue - (int)previousTime;
            }
        }
        else
        {
            time += (uint32_t)round(deltaTime);
        }
    }
}

uint32_t Clock::GetTime()
{
    return time;
}

uint32_t Clock::GetInitialTime()
{
    return initialTime;
}

float Clock::GetDeltaTime()
{
    if (paused)
    {
        return 0;
    }
    if (overflow != 0)
    {
        return (float)((int)time - overflow) / 1000.0f;
    }
    return (float)((int)time - (int)previousTime) / 1000.0f;
}

void Clock::SetTime(uint32_t pos)
{
    time = pos;
    previousTime = pos;
    overflow = 0;
}

void Clock::SetWrap(uint32_t value)
{
    wrapValue = value;
}

uint32_t Clock::GetWrap()
{
    return wrapValue;
}

///
/// Timer definitions
///

Timer::Timer(Clock* refClock)
{
    clock = refClock;
}

Timer::Timer(const Timer& source)
{
    paused = source.paused;
    started = source.started;
    startTicks = source.startTicks;
    pausedTicks = source.pausedTicks;
    if (source.clock != nullptr)
    {
        clock = new Clock(*source.clock);
    }
}
Timer& Timer::operator=(const Timer& source)
{
    *this = Timer(source);
    return *this;
}

void Timer::Start()
{
    started = true;
    paused = false;
    pausedTicks = 0;
    if (clock != nullptr)
    {
        startTicks = clock->GetTime();
    }
    else
    {
        startTicks = millis();
    }
}

void Timer::Stop()
{
    started = false;
    paused = false;
    startTicks = 0;
    pausedTicks = 0;
}

void Timer::Pause()
{
    if (started && !paused)
    {
        paused = true;
        if (clock != nullptr)
        {
            pausedTicks = clock->GetTime() - startTicks;
        }
        else
        {
            pausedTicks = millis() - startTicks;
        }
        startTicks = 0;
    }
}

void Timer::Resume()
{
    if (started && paused)
    {
        paused = false;
        if (clock != nullptr)
        {
            startTicks = clock->GetTime() - pausedTicks;
        }
        else
        {
            startTicks = millis() - pausedTicks;
        }
        pausedTicks = 0;
    }
}

uint32_t Timer::GetTicks()
{
    uint32_t time = 0;
    if (started)
    {
        if (paused)
        {
            time = pausedTicks;
        }
        else
        {
            if (clock != nullptr)
            {
                time = clock->GetTime() - startTicks;
            }
            else
            {
                time = millis() - startTicks;
            }
        }
    }
    return time;
}

bool Timer::IsStarted()
{
    return started;
}
bool Timer::IsPaused()
{
    return paused;
}

uint32_t GetMS(float seconds)
{
    return (uint32_t)(seconds * 1000.0f);
}

float GetSeconds(uint32_t ms)
{
    return (float)ms / 1000.0f;
}
