#include "config.h"
#include "watch.h"

void Watch::Init()
{
    driver->begin();
    driver->tft->setTextColor(TFT_GREEN);
    timer.Start();
}

void Watch::Update()
{
    uint32_t startTicks = timer.GetTicks();

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
                updateDisplay = true;
            }
            break;
        default:
            break;
        }
    }

    // Render to the display
    if (updateDisplay && display.IsEnabled())
    {
        // Render to display
        //display.RenderPresent();
    }

    // Update at specified frame rate
    uint32_t delta = timer.GetTicks() - startTicks;
    if (delta < frameDelayMS)
    {
        delay(frameDelayMS - delta);
        delta = frameDelayMS;
    }

    clock.Update((float)delta / 1000.0f);
}
