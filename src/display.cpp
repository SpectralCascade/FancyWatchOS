#include "config.h"

#include "display.h"
#include "surface.h"

Display::Display(TTGOClass* watch)
{
    device = watch;
    renderBuffer = new Surface(320, 240);
}

void Display::Enable()
{
    if (!enabled)
    {
        // Power up the backlight
        device->power->setPowerOutPut(AXP202_LDO2, true);

        // Initialise it
        device->openBL();
        device->displayWakeup();
        enabled = true;
    }
}

void Display::Disable()
{
    if (enabled)
    {
        device->displaySleep();
        // Turn off power to the screen
        device->power->setPowerOutPut(AXP202_LDO2, false);
        enabled = false;
    }
}

void Display::SetBrightness(float percent)
{
    brightness = (uint8_t)MapRange(percent, 0, 1, 0, 255);
}

float Display::GetBrightness()
{
    return brightness;
}

bool Display::IsEnabled()
{
    return enabled;
}

void Display::Clear(Color color)
{
    renderBuffer->Clear(color);
}

void Display::RenderPresent()
{
    device->tft->startWrite();
    device->tft->pushPixels(renderBuffer->GetPixels(), renderBuffer->GetWidth() * renderBuffer->GetHeight());
    device->tft->endWrite();
}

TFT_eSPI* Display::GetTFT()
{
    return device->tft;
}
