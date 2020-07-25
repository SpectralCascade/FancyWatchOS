/*
 * WakeupFormPEKKey: Use AXP202 interrupt pin to wake up T-Watch
 * Copyright 2020 Lewis he
 */

#include "config.h"

enum PowerEvent
{
    POWER_BUTTON = 0,
    POWER_CONNECT,
    POWER_DISCONNECT
};

TTGOClass *watch;
bool awake = false;
bool togglePower = false;
int powerEventCount = 0;
int buttonPressCount = 0;
int lastPowerEvent = POWER_CONNECT;
const unsigned int POWER_CHECK_INTERVAL = 32;

void WatchWakeUp()
{
    awake = true;
    ToggleDisplay(true);
}

void UpdateInfo()
{
    watch->tft->fillScreen(TFT_BLACK);
    watch->tft->setTextColor(TFT_GREEN);
    watch->tft->setCursor(0, 0);
    char dest[100] = {'\0'};
    sprintf(dest, "Power events: %d | Button events: %d", powerEventCount, buttonPressCount);
    watch->tft->println(dest);
    watch->tft->println("\n\nWoken by:\n");
    watch->tft->println(lastPowerEvent == POWER_BUTTON ? "POWER BUTTON" : (lastPowerEvent == POWER_CONNECT ? "POWER CONNECT" : "POWER DISCONNECT"));
}

void ToggleDisplay(bool on)
{
    if (on)
    {
        // Power up the backlight
        watch->power->setPowerOutPut(AXP202_LDO2, true);
        watch->bl->begin();
        watch->bl->on();
        // Wake up the TFT
        watch->displayWakeup();
    }
    else
    {
        watch->displaySleep();
        // Turn off the screen
        watch->power->setPowerOutPut(AXP202_LDO2, false);
    }
}

void WatchSleep()
{
    awake = false;
    ToggleDisplay(false);
    // Go into deep-sleep mode
    // Use ext1 for external wakeup
    //esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
    //esp_deep_sleep_start();
}

// ISR for handling AXP202 power interrupts, such as plugging in a power cable, pressing the power button etc.
void OnPowerInterrupt()
{
    togglePower = true;
}

void InitInterrupts(TTGOClass* ttgoWatch)
{
    // Setup interrupt to indicate when the power state changes.
    pinMode(AXP202_INT, INPUT_PULLUP);
    attachInterrupt(AXP202_INT, OnPowerInterrupt, FALLING);

    // Enable the power interrupts
    ttgoWatch->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ | AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ | AXP202_CHARGING_IRQ, true);
    //  Clear the current interrupt status
    ttgoWatch->power->clearIRQ();
}

void setup()
{
    // Get object representing the watch
    watch = TTGOClass::getWatch();

    // Initialize the the watch
    watch->begin();

    InitInterrupts(watch);

    WatchWakeUp();
}

void loop()
{
    delay(POWER_CHECK_INTERVAL);

    if (togglePower)
    {
        watch->power->readIRQ();
        if (watch->power->isVbusPlugInIRQ())
        {
            lastPowerEvent = POWER_CONNECT;
        }
        else if (watch->power->isVbusRemoveIRQ())
        {
            lastPowerEvent = POWER_DISCONNECT;
        }
        else if (watch->power->isPEKShortPressIRQ())
        {
            lastPowerEvent = POWER_BUTTON;
            buttonPressCount++;

            // Power button is pressed, toggle the display
            if (awake)
            {
                WatchSleep();
            }
            else
            {
                WatchWakeUp();
            }
        }
        powerEventCount++;
        togglePower = true;
        watch->power->clearIRQ();

        UpdateInfo();

        togglePower = false;
    }
}



