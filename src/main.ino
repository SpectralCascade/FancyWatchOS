/*
 * WakeupFormPEKKey: Use AXP202 interrupt pin to wake up T-Kernel
 * Copyright 2020 Lewis he
 */

#include "config.h"
#include "display.h"
#include "kernel.h"
#include "utils.h"
#include "Apps/homestead.h"

// The FancyWatchOS runtime that coordinates I/O and applications.
Kernel* kernel;

// Events queue
QueueHandle_t events;
bool powerIRQ = false;
bool rtcIRQ = false;

void setup()
{

#ifdef LOG_SERIAL
    Serial.begin(9600);
#endif

    // Setup the event queue
    // Maximum number of events allowed per frame. Pretty high to deal with all the event types we could get.
    events = xQueueCreate(256, sizeof(Event));

    // Initialise the watch
    kernel = new Kernel(TTGOClass::getWatch(), events);

    InitInterrupts(kernel->GetDriver());

    // Init display
    kernel->GetDisplay()->Enable();
    kernel->GetDisplay()->GetTFT()->setTextColor(TFT_WHITE);

    Log("Setup kernel.");

    // Finally, start the main app.
    kernel->StartApp(new Homestead());

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

    //
    // RTC interrupts
    //
    pinMode(RTC_INT, INPUT_PULLUP);
    attachInterrupt(RTC_INT, [] { rtcIRQ = true; }, FALLING);
    device->rtc->disableAlarm();

    // Initialise RTC based on compile time.
    device->rtc->check();

}

void loop()
{
    // Handle interrupts and pass to the events queue.

    //
    // Power
    //
    if (powerIRQ)
    {
        AXP20X_Class* power = kernel->GetDriver()->power;
        power->readIRQ();

        Event e;
        if (power->isVbusPlugInIRQ())
        {
            e.type = EVENT_POWER_CONNECT;
        }
        else if (power->isVbusRemoveIRQ())
        {
            e.type = EVENT_POWER_DISCONNECT;
        }
        else if (power->isPEKShortPressIRQ())
        {
            e.type = EVENT_POWER_BUTTON;
        }
        else
        {
            e.type = EVENT_POWER_CHARGE;
        }
        power->clearIRQ();
        // Add the event to the queue
        xQueueSend(events, &e, portMAX_DELAY);

        powerIRQ = false;
    }
    if (rtcIRQ)
    {
        PCF8563_Class* rtc = kernel->GetDriver()->rtc;
        detachInterrupt(RTC_INT);

        Event e;
        if (rtc->alarmActive())
        {
            // What to do here? Reset it?
            //rtc->resetAlarm();
        }
        if (rtc->isTimerEnable() && rtc->isTimerActive())
        {
            // What to do here? Clear it?
        }

        // Convert RTC date to POD type.
        RTC_Date date = rtc->getDateTime();
        e.rtc.second = date.second;
        e.rtc.minute = date.minute;
        e.rtc.hour = date.hour;
        e.rtc.day = date.day;
        e.rtc.month = date.month;
        e.rtc.year = date.year;

        // Add the event to the queue
        xQueueSend(events, &e, portMAX_DELAY);

        rtcIRQ = false;
    }

    // Update the watch runtime
    kernel->Update();
}



