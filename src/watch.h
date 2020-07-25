#ifndef WATCH_H
#define WATCH_H

#include "time.h"
#include "display.h"

// Forward declaration
class TTGOClass;

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

class Watch
{
public:
    Watch(TTGOClass* device, QueueHandle_t eventQueue) : display(device), driver(device), events(eventQueue)
    {
        // Initialize the watch
        // Start timing for frame rate control
        //timer.Start();
    }

    void Init();

    void Update();

    void ShowDebugInfo();

    void Log(char* message);

    Display display;

    TTGOClass* driver;

private:
    QueueHandle_t events;

    // Timing and frame rate management
    Clock clock;
    Timer timer;
    uint32_t fps = 30;
    uint32_t frameDelayMS = 33;

};

#endif // WATCH_H
