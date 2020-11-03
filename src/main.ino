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
bool touchIRQ = false;

// The default library only supports up to 2 touches at a time, though in theory it could support more.
TouchEvent lastTouches[2] = { { 0xFF, -1, -1 }, { 0xFF, -1, -1 } };
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

    // Disable unused power outputs.
    /*
        From https://github.com/tuupola/esp_twatch2020

        DC-DC1	PWM charger.
        DC-DC2	Not used 0.7V to 2.275V, 1.6A.
        DC-DC3	ESP32, 0.7V to 3.5V, 1.2A. Always enable!
        LDO1	Always on 30mA.
        LDO2 1)	Display backlight 1.8V to 3.3V, 200mA.
        LDO3 2)	Audio power 0.7V to 3.5V, 200mA.
        LDO4	Not used 1.8V to 3.3V, 200mA.
        LDO5	Not used 1.8V to 3.3V, 50mA.
    */
    kernel->GetDriver()->power->setPowerOutPut(AXP202_EXTEN, AXP202_OFF);
    kernel->GetDriver()->power->setPowerOutPut(AXP202_DCDC2, AXP202_OFF);
    kernel->GetDriver()->power->setPowerOutPut(AXP202_LDO3, AXP202_OFF);
    kernel->GetDriver()->power->setPowerOutPut(AXP202_LDO4, AXP202_OFF);

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

    //
    // Touch interrupts
    //
    pinMode(TOUCH_INT, INPUT_PULLUP);
    attachInterrupt(TOUCH_INT, [] { touchIRQ = true; }, FALLING);

}

void loop()
{
    // Handle interrupts and pass to the events queue.
    bool active = kernel->IsActive();

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

        // Power interrupts always activate the kernel.
        kernel->SetActive(true);

        powerIRQ = false;
    }

    //
    // RTC
    //
    if (rtcIRQ)
    {
        PCF8563_Class* rtc = kernel->GetDriver()->rtc;
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
    }

    //
    // Touches
    //
    if (touchIRQ)
    {
        if (!(kernel->enabledEventsMask & (EVENT_TOUCH_BEGIN | EVENT_TOUCH_CHANGE | EVENT_TOUCH_END)))
        {
            touchIRQ = false;
        }
        else
        {
            // TODO: handle multiple touch interrupts in a single frame?
            FT5206_Class* touch = kernel->GetDriver()->touch;

            uint8_t touches = touch->touched();
            Event e[2];
            for (uint8_t i = 0; i < touches; i++)
            {
                // Grab touch data
                e[i].touch.touchID = i;
                TP_Point point = touch->getPoint(i);
                e[i].touch.x = point.x;
                e[i].touch.y = point.y;

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

    if (active)
    {
        // Update the watch runtime
        kernel->Update();
    }
    else
    {
        // May as well save some processing cycles while inactive, interrupts will still be handled.
        vTaskDelay(3);
    }
}



