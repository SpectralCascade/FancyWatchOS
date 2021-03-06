#ifndef WATCH_H
#define WATCH_H

#include "display.h"
#include "time.h"
#include <functional>

// Note: MAX_APPS should never be more than 256
#define MAX_APPS 16

// Maximum number of input events that can be queued.
#define MAX_EVENTS 256

// The time in milliseconds between display refreshes.
#define DISPLAY_REFRESH_DELAY 33

// The maximum stack depth of an application.
#define MAX_APP_STACK 256

// TODO: replace with better timeout system.
// 6 second timeout of the display without any inputs.
#define DISPLAY_TIMEOUT 6000

// Forward declarations
class TTGOClass;
class Application;
class Renderer;

// All input event types.
enum EventType
{
    EVENT_UNKNOWN             = 0b00000000000000000000000000000000,
    EVENT_POWER_CONNECT       = 0b00000000000000000000000000000001,
    EVENT_POWER_CHARGE        = 0b00000000000000000000000000000010,
    EVENT_POWER_DISCONNECT    = 0b00000000000000000000000000000100,
    EVENT_POWER_BUTTON        = 0b00000000000000000000000000001000,
    EVENT_RTC_ALARM           = 0b00000000000000000000000000010000,
    EVENT_RTC_TIMER           = 0b00000000000000000000000000100000,
    EVENT_TOUCH_BEGIN         = 0b00000000000000000000000001000000,
    EVENT_TOUCH_END           = 0b00000000000000000000000010000000,
    EVENT_TOUCH_CHANGE        = 0b00000000000000000000000100000000,
    EVENT_BMA_TILT            = 0b00000000000000000000001000000000,
    EVENT_BMA_DOUBLE_TAP      = 0b00000000000000000000010000000000,
    EVENT_BMA_STEP_COUNT      = 0b00000000000000000000100000000000
};

// All event groups
struct PowerEvent
{
    // TODO: useful power data
};
struct RealtimeClockEvent
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
};
struct TouchEvent
{
    uint8_t touchID;
    uint16_t x;
    uint16_t y;
};

// Structure containing a union defining different types of events with their meta data.
struct Event
{
    int32_t type;
    union {
        PowerEvent power;
        RealtimeClockEvent rtc;
        TouchEvent touch;
    };
};

/// Main runtime. Deals with running user applications.
class Kernel
{
public:
    friend Renderer;

    Kernel(TTGOClass* device, QueueHandle_t eventQueue);
    ~Kernel();

    // Update the system.
    void Update();

    // Run a low-level system task.
    // Don't do anything stupid and be as quick as possible, i.e. copy data and exit.
    // Useful for interacting with hardware and so on without breaking other applications.
    // Runs on whatever thread called it and blocks until complete.
    void RunSystemTask(std::function<void()> task);

    // Starts an app and returns an ID for it. Returns -1 on failure (e.g. maximum number of apps are running).
    int StartApp(Application* app, bool foreground = true, int argc = 0, char* argv[] = NULL);

    // Kill an app that is running. Set force = true to skip calling Application::OnStop().
    // Returns the application that has been killed so it can be freed from memory if you wish.
    Application* KillApp(int id, bool force = false);

    // Enable or disable the kernel to save power. Setting inactive means apps don't run at all until reactivated,
    // even input events, as Kernel::Update() is not called.
    void SetActive(bool active);

    // Is this kernel operating?
    bool IsActive();

    // Enable a specific event.
    void EnableEvents(int32_t type);

    // Disable a specific event.
    void DisableEvents(int32_t type);

    // Bitmask indicating which event types are enabled.
    int32_t enabledEventsMask = 0xFFFFFFFF;

    // Causes the watch to enter light-sleep power saving mode at the end of the next update.
    void EnterSleep();

    Display display;

    TTGOClass* driver;

private:
    // Timer for putting the watch to sleep after some time without any input events.
    Timer napTimer;

    // Input event queue.
    QueueHandle_t events;

    // All running applications, up to MAX_APPS as a stack.
    Application* apps[MAX_APPS] = { nullptr };

    // The number of apps that are currently running.
    uint16_t totalApps = 0;

    // Should the kernel update?
    bool active = true;

    // Used to ensure the watch cannot be deactivated immediately after being activated.
    bool wasActive = true;

    // Should the watch sleep at the end of the next update?
    bool sleepMode = false;

    // Timing and frame rate management
    Timer renderTimer;

};

#endif // WATCH_H
