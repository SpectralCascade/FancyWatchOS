#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include "color.h"

// Forward declarations
class TTGOClass;
class TFT_eSPI;
class Surface;

/// TFT display API
class Display
{
public:
    Display(TTGOClass* watch);

    // Call this after the constructor
    void Init();

    void Enable();
    void Disable();

    void SetBrightness(float percent);
    float GetBrightness();

    bool IsEnabled();

    // Clears the current display with a given color.
    void Clear(Color color);

    // Renders the surface buffer.
    void RenderPresent();

    // Returns a pointer to the TFT_eSPI instance.
    TFT_eSPI* GetTFT();

private:
    // Buffer for rendering to the TFT.
    Surface* renderBuffer;

    // Reference to the device implementation
    TTGOClass* device;

    // Whether the display is enabled
    bool enabled = false;

    // How bright the display is
    uint8_t brightness;

};

#endif // DISPLAY_H
