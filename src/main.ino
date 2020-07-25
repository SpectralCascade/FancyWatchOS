/*
 * WakeupFormPEKKey: Use AXP202 interrupt pin to wake up T-Watch
 * Copyright 2020 Lewis he
 */

#include "config.h"
#include "display.h"
#include "watch.h"

// The watch system itself
Watch* watch;
// Events queue
QueueHandle_t events;
bool powerIRQ = false;

void setup()
{
    // Setup the event queue
    // Maximum number of events allowed per frame. Pretty high to deal with all the event types we could get.
    events = xQueueCreate(256, sizeof(Event));

    // Initialise the watch
    watch = new Watch(TTGOClass::getWatch(), events);
    watch->Init();

    InitInterrupts(watch->driver);

    // Init display
    watch->display.Enable();
}

void InitInterrupts(TTGOClass* device)
{
    //
    // Power interrupts
    //
    pinMode(AXP202_INT, INPUT_PULLUP);
    attachInterrupt(AXP202_INT, [] { powerIRQ = true; }, FALLING);
    device->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ | AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ | AXP202_CHARGING_IRQ, true);
    device->power->clearIRQ();
}

void loop()
{
    if (powerIRQ)
    {
        AXP20X_Class* power = watch->driver->power;
        power->readIRQ();

        Event e;
        if (power->isVbusPlugInIRQ())
        {
            e.type = EVENT_POWER_CONNECT;
            watch->Log("Power connect!");
        }
        else if (power->isVbusRemoveIRQ())
        {
            e.type = EVENT_POWER_DISCONNECT;
            watch->Log("Power disconnect!");
        }
        else if (power->isPEKShortPressIRQ())
        {
            e.type = EVENT_POWER_BUTTON;
            watch->Log("Power button!");
        }
        else
        {
            e.type = EVENT_POWER_CHARGE;
            watch->Log("Power charge!");
        }
        power->clearIRQ();
        // Add the event to the queue
        xQueueSend(events, &e, portMAX_DELAY);

        powerIRQ = false;
    }

    watch->Update();
}



