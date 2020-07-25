#ifndef TIME_H
#define TIME_H

/// A special clock that measures relative time passed
class Clock
{
public:
    /// Takes the 'absolute' time at which the clock began, in milliseconds.
    explicit Clock(uint32_t startTimeMS = 0);

    /// Updates the time passed on the clock according to the time scale factor. Returns true if wrapping occurred during the update.
    bool Update(float deltaTime);

    /// Pause/Resume time.
    void SetPaused(bool pause);

    /// Return pause state.
    bool IsPaused();

    /// Stretch clock time by some scale factor.
    void Scale(float scaleFactor);

    /// Returns the scale factor.
    float GetScaleFactor();

    /// Steps forward or backwards in time by a number of frames; defaults to 1/60th of a second per frame.
    void StepFrames(int frames = 1, float framePeriod = 1.0f / 60.0f);

    /// Returns relative time passed in milliseconds.
    uint32_t GetTime();

    /// Returns the 'absolute' time that the clock began in milliseconds.
    uint32_t GetInitialTime();

    /// Returns the relative time difference between Update() calls.
    float GetDeltaTime();

    /// Sets the time. DeltaTime is set to 0.
    void SetTime(uint32_t pos);

    /// Make this clock wraparound if it goes over this value. Set to 0 to disable wrapping.
    void SetWrap(uint32_t value);

    /// Returns the wraparound value.
    uint32_t GetWrap();

private:
    /// Initial, 'absolute' time at which the clock began.
    uint32_t initialTime = 0;

    /// Relative time passed since clock began in milliseconds.
    uint32_t time = 0;

    /// Last value of time.
    uint32_t previousTime = 0;

    /// Scale factor to stretch/compress clock by.
    float scale = 1.0f;

    /// Whether or not the clock is paused.
    bool paused = false;

    /// The maximum value of time. Ignored if 0.
    uint32_t wrapValue = 0;

    /// Wrapping can cause issues with calculating delta time; this variable ensures no accuracy is lost if wrapping occurs.
    int overflow = 0;

};

/// A simple timer, like a stop watch; can only be used for positive timing.
/// As such, use of this class is limited to timeframes that aren't reversable.
class Timer
{
public:
    /// Takes a clock instance to provide timing ticks - otherwise defaults to absolute time.
    Timer(Clock* refClock = nullptr);
    Timer(const Timer& source);
    Timer& operator=(const Timer& source);

    /// Timer actions; all parameters are in milliseconds and generally correspond to time passed
    /// since some arbitrary clock began - if no clock object is used, utilise SDL_GetTicks().
    void Start();
    void Stop();
    void Pause();
    void Resume();

    /// Return the relative time since the timer has been started.
    uint32_t GetTicks();

    /// Return timer flags.
    bool IsStarted();
    bool IsPaused();

private:
    /// Reference clock.
    Clock* clock;

    /// Time at which the timer started, in milliseconds.
    uint32_t startTicks = 0;

    /// Ticks stored while timer is paused.
    uint32_t pausedTicks = 0;

    /// Timer flags.
    bool paused = false;
    bool started = false;

};

/// Converts seconds to milliseconds.
uint32_t GetMS(float seconds);
/// Converts milliseconds to seconds.
float GetSeconds(uint32_t ms);

#endif // TIME_H
