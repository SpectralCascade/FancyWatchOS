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
// TODO: use thread safe bit flags instead
bool powerIRQ = false;
bool rtcIRQ = false;
bool touchIRQ = false;
bool bmaIRQ = false;

// The default library only supports up to 2 touches at a time, though in theory it could support more.
TouchEvent lastTouches[2] = { { 0xFF, 0xFFFF, 0xFFFF }, { 0xFF, 0xFFFF, 0xFFFF } };
uint8_t lastNumTouches = 0;

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

    InitInterrupts(kernel->driver);

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
    /*pinMode(RTC_INT, INPUT_PULLUP);
    attachInterrupt(RTC_INT, [] { rtcIRQ = true; }, FALLING);
    device->rtc->disableAlarm();*/

    // Initialise RTC based on compile time.
    device->rtc->check();

    //
    // Touch interrupts
    //

    pinMode(TOUCH_INT, INPUT_PULLUP);
    attachInterrupt(TOUCH_INT, [] () { touchIRQ = true; }, FALLING);

    //
    // BMA interrupts.
    //

    // Detects when you tilt the watch.
    device->bma->enableTiltInterrupt();
    // "Double-tap" interrupt - detects motion from double tapping the watch quickly.
    // Removed for now. While neat, in practice it unintendedly fires more often than not.
    //device->bma->enableWakeupInterrupt();
    // Assumes the sensor is configured, otherwise uses default settings.
    device->bma->attachInterrupt();

    pinMode(BMA423_INT1, INPUT);
    attachInterrupt(BMA423_INT1, [] () { bmaIRQ = true; }, RISING);

}

void loop()
{
    // Handle interrupts and pass to the events queue.

    //
    // Power
    //
    if (powerIRQ)
    {
        AXP20X_Class* power = kernel->driver->power;
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

        // Power interrupts always activate the kernel.
        kernel->SetActive(true);

        powerIRQ = false;
    }

    //
    // RTC
    //
    /*if (rtcIRQ)
    {
        PCF8563_Class* rtc = kernel->driver->rtc;
        detachInterrupt(RTC_INT);

        Event e;
        // TODO: Handle both timer and alarm at same time?
        if (rtc->alarmActive())
        {
            // What to do here? Reset it?
            //rtc->resetAlarm();
            e.type = EVENT_RTC_ALARM;
        }
        if (rtc->isTimerEnable() && rtc->isTimerActive())
        {
            // What to do here? Clear it?
            e.type = EVENT_RTC_TIMER;
        }

        // Convert RTC date to POD type.
        RTC_Date date = rtc->getDateTime();
        e.rtc.second = date.second;
        e.rtc.minute = date.minute;
        e.rtc.hour = date.hour;
        e.rtc.day = date.day;
        e.rtc.month = date.month;
        e.rtc.year = date.year;

        xQueueSend(events, &e, portMAX_DELAY);

        rtcIRQ = false;
    }*/

    //
    // Touches
    //
    if (touchIRQ)
    {
        Log("DEBUG: Touch IRQ triggered...");
        if (!(kernel->enabledEventsMask & (EVENT_TOUCH_BEGIN | EVENT_TOUCH_CHANGE | EVENT_TOUCH_END)))
        {
            touchIRQ = false;
        }
        else
        {
            // TODO: handle multiple touch interrupts in a single frame?
            CapacitiveTouch* touch = kernel->driver->touch;

            uint8_t touches = touch->getTouched();
            Event e[2];
            uint16_t x, y;
            touch->getPoint(x, y);
            Log("Total touches = %d, returned point: %d, %d", touches, x, y);
            for (uint8_t i = 0; i < touches; i++)
            {
                // Grab touch data
                e[i].touch.touchID = i;
                touch->getPoint(e[i].touch.x, e[i].touch.y);

                // Check which type of touch event this is.
                if (i >= lastNumTouches)
                {
                    e[i].type = EVENT_TOUCH_BEGIN;
                    xQueueSend(events, &e[i], portMAX_DELAY);
                }
                else if (e[i].touch.x != lastTouches[i].x || e[i].touch.y != lastTouches[i].y)
                {
                    e[i].type = EVENT_TOUCH_CHANGE;
                    xQueueSend(events, &e[i], portMAX_DELAY);
                }

                lastTouches[i] = e[i].touch;
            }

            // Touch end event if there are less touches.
            for (uint8_t i = lastNumTouches; i > touches; i--)
            {
                uint8_t index = i - 1;
                e[index].type = EVENT_TOUCH_END;
                e[index].touch = lastTouches[index];
                xQueueSend(events, &e[index], portMAX_DELAY);
            }

            // Only set touchIRQ to false if all touches are released.
            if (!touches)
            {
                touchIRQ = false;
            }
            lastNumTouches = touches;
        }

    }

    //
    // BMA sensor
    //
    if (bmaIRQ)
    {
        bool read = false;

        Event e;
        e.type = 0;
        do
        {
            read = kernel->driver->bma->readInterrupt();

            if (kernel->driver->bma->isTilt())
            {
                e.type = EVENT_BMA_TILT;
            }
            else if (kernel->driver->bma->isDoubleClick())
            {
                e.type = EVENT_BMA_DOUBLE_TAP;
            }
            else if (kernel->driver->bma->isStepCounter())
            {
                e.type = EVENT_BMA_STEP_COUNT;
            }
            else
            {
                // TODO: other events
                e.type = 0;
            }

            if (e.type != 0)
            {
                xQueueSend(events, &e, portMAX_DELAY);
            }

            if (e.type == EVENT_BMA_TILT || e.type == EVENT_BMA_DOUBLE_TAP)
            {
                kernel->SetActive(true);
            }

        } while (!read);

        bmaIRQ = false;
    }

    // TODO: if watch is not active, sleep again!
    if (kernel->IsActive())
    {
        // Update the watch runtime
        kernel->Update();
    }
    else
    {
        // May as well save some processing cycles while inactive, interrupts will still be handled.
        vTaskDelay(DISPLAY_REFRESH_DELAY);
    }

    // Empty the events queue
    while (uxQueueMessagesWaiting(events) > 0)
    {
        Event e;
        xQueueReceive(events, &e, portMAX_DELAY);
    }
}



