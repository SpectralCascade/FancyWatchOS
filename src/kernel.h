#ifndef WATCH_H
#define WATCH_H

#include "display.h"
#include "time.h"
#include "renderer.h"
#include <functional>

// Note: MAX_APPS should never be more than 256
#define MAX_APPS 16

// Maximum number of input events that can be queued.
#define MAX_EVENTS 256

// The time in milliseconds between display refreshes.
#define DISPLAY_REFRESH_DELAY 33

// The maximum stack depth of an application.
#define MAX_APP_STACK 256

// Forward declarations
class TTGOClass;
class Application;
class Renderer;

// All input event types.
enum EventType
{
    EVENT_UNKNOWN = 0,
    EVENT_POWER_CONNECT = 1,
    EVENT_POWER_CHARGE = 2,
    EVENT_POWER_DISCONNECT = 3,
    EVENT_POWER_BUTTON = 4
};

// All event groups
struct PowerEvent
{
    // TODO: useful power data
};

// Structure containing a union defining different types of events with their meta data.
struct Event
{
    int32_t type;
    union {
        PowerEvent power;
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

    // Don't call this.
    Display* GetDisplay();

    // Or this.
    TTGOClass* GetDriver();

private:
    void SuspendAllApps();
    void ResumeAllApps();

    Display display;
    TTGOClass* driver;

    // Input event queue.
    QueueHandle_t events;

    // All running applications, up to MAX_APPS.
    Application* apps[MAX_APPS] = { nullptr };

    // Corresponding task handles for each app.
    TaskHandle_t taskHandles[MAX_APPS] = { nullptr };

    // The app that is currently in the foreground.
    uint8_t mainApp = 0;

    // Timing and frame rate management
    Timer renderTimer;

};

#endif // WATCH_H
