#include "config.h"

#include "display.h"

Surface gRenderBuffer;

void Display::Init(TTGOClass* watch)
{
    device = watch;
    gRenderBuffer.Init(240, 240, PF_RGB565);
}

void Display::Destroy()
{
    gRenderBuffer.Destroy();
}

void Display::Enable()
{
    if (!enabled)
    {
        // Power up the backlight
        device->power->setPowerOutPut(AXP202_LDO2, true);

        // Initialise it again
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

void Display::Clear(uint16_t color)
{
    gRenderBuffer.Clear(color);
}

void Display::RenderPresent()
{
    device->tft->drawBitmap(0, 0, (uint8_t*)gRenderBuffer.GetPixels(), gRenderBuffer.GetWidth(), gRenderBuffer.GetHeight(), TFT_BLUE);
}

TFT_eSPI* Display::GetTFT()
{
    return device->tft;
}
