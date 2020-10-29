#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include "color.h"
#include "surface.h"

// Forward declarations
class TTGOClass;
class TFT_eSPI;

// Global surface for rendering
extern Surface gRenderBuffer;

/// TFT display API
class Display
{
public:
    void Init(TTGOClass* watch);
    void Destroy();

    void Enable();
    void Disable();

    void SetBrightness(float percent);
    float GetBrightness();

    bool IsEnabled();

    // Clears the current display with a given color.
    void Clear(uint16_t color);

    // Draws the render buffer to the display.
    void RenderPresent();

    // Returns a pointer to the TFT_eSPI instance.
    TFT_eSPI* GetTFT();

private:
    // Reference to the device implementation
    TTGOClass* device;

    // Whether the display is enabled
    bool enabled = false;

    // How bright the display is
    uint8_t brightness;

};

#endif // DISPLAY_H
