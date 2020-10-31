#include "config.h"

#include "display.h"

void Display::Init(TTGOClass* watch)
{
    device = watch;
    renderBuffer.Init(240, 240, PF_RGB565);
#ifdef RENDER_DMA
    // Use DMA for fast rendering.
    dmaBuffer.Init(240, 240, PF_RGB565);
    GetTFT()->initDMA();
    GetTFT()->setAddrWindow(0, 0, 240, 240);
#endif // RENDER_DMA

#ifdef OPTIMISED_RENDERING
    tftspi = new Arduino_ST7789(27, -1, 5);
    tftspi->init();
#endif // OPTIMISED_RENDERING
}

void Display::Destroy()
{
    renderBuffer.Destroy();
#ifdef RENDER_DMA
    dmaBuffer.Destroy();
#endif // RENDER_DMA
#ifdef OPTIMISED_RENDERING
    delete tftspi;
#endif // OPTIMISED_RENDERING
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
    renderBuffer.Clear(color);
}

void Display::RenderPresent()
{
#ifdef RENDER_DMA
    device->tft->pushImageDMA(0, 0, 240, 240, (uint16_t*)renderBuffer.GetPixels(), (uint16_t*)dmaBuffer.GetPixels());
#else
#ifdef OPTIMISED_RENDERING
    tftspi->drawImage(0, 0, renderBuffer.GetWidth(), renderBuffer.GetHeight(), (uint16_t*)renderBuffer.GetPixels());
#else
    device->tft->pushRect(0, 0, renderBuffer.GetWidth(), renderBuffer.GetHeight(), (uint16_t*)renderBuffer.GetPixels());
#endif // OPTIMISED_RENDERING
#endif // RENDER_DMA

}

TFT_eSPI* Display::GetTFT()
{
    return device->tft;
}

Surface* Display::GetBuffer()
{
    return &renderBuffer;
}
