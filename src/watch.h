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
    Watch(TTGOClass* device, QueueHandle_t eventQueue);
    ~Watch();

    void Update();

    Display display;

    TTGOClass* driver;

private:
    QueueHandle_t events;

    // Timing and frame rate management
    Clock clock;
    Timer timer;
    uint32_t frameDelayMS = 33;

};

#endif // WATCH_H
