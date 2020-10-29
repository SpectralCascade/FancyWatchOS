#include "config.h"
#include "watch.h"
#include "utils.h"
#include "display.h"

Watch::Watch(TTGOClass* device, QueueHandle_t eventQueue)
{
    // Initialise the watch
    driver = device;
    driver->begin();
    // Setup display
    events = eventQueue;
    display.Init(driver);
}

Watch::~Watch()
{
    display.Destroy();
}

void Watch::Update()
{
    uint32_t startTicks = timer.GetTicks();

    // Should the watch deep sleep at the end of this update?
    bool deepSleep = false;

    // Should the display be updated?
    bool updateDisplay = false;

    // Check for input events
    Event e;
    while (uxQueueMessagesWaiting(events) > 0)
    {
        xQueueReceive(events, &e, portMAX_DELAY);
        switch (e.type)
        {
        case EVENT_POWER_BUTTON:
            if (display.IsEnabled())
            {
                display.Disable();
            }
            else
            {
                display.Enable();
            }
            break;
        default:
            break;
        }
    }
    updateDisplay = true;

    // Render to the display
    if (!deepSleep && updateDisplay && display.IsEnabled())
    {
        if (timer.GetTicks() > 3000)
        {
            display.Clear(TFT_RED);
            timer.Start();
        }
        else if (timer.GetTicks() > 2000)
        {
            display.Clear(TFT_BLUE);
        }
        else
        {
            display.Clear(TFT_WHITE);
        }
        //display->Clear(TFT_WHITE);

        display.RenderPresent();

        //driver->tft->setCursor(0, 0);
        //driver->tft->printf("FPS: %f\n", 1.0f / clock.GetDeltaTime());
    }

    // Update at specified frame rate
    uint32_t delta = timer.GetTicks() - startTicks;
    if (delta < frameDelayMS)
    {
        delay(frameDelayMS - delta);
        delta = frameDelayMS;
    }

    clock.Update((float)delta / 1000.0f);

    if (deepSleep)
    {
        display.Disable();

        timer.Pause();

        esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);

        esp_deep_sleep_start();
    }
}
