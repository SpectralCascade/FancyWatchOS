#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include "color.h"
#include "surface.h"

//#define OPTIMISED_RENDERING

#ifdef OPTIMISED_RENDERING
#include "Arduino_ST7789_Fast.h"
#endif // OPTIMISED_RENDERING

//#define RENDER_DMA

// Forward declarations
class TTGOClass;
class TFT_eSPI;
class Kernel;

/// TFT display API
class Display
{
public:
    void Init(TTGOClass* watch);
    void Destroy();

    void Enable();
    void Disable();

    void SetBrightness(float percent);
    void SetBrightnessLevel(uint8_t level);
    float GetBrightness();
    uint8_t GetBrightnessLevel();

    bool IsEnabled();

    // Clears the current display with a given color.
    void Clear(uint16_t color);

    // Set the default draw colour.
    void SetDrawColor(uint16_t color);

    // Return the default draw color.
    uint16_t GetDrawColor();

    // Draws the render buffer to the display.
    void RenderPresent();

    // Returns the raw render buffer.
    Surface* GetBuffer();

    // Returns a pointer to the TFT_eSPI instance.
    TFT_eSPI* GetTFT();

private:
    // Reference to the device implementation
    TTGOClass* device;

    // The buffer used for rendering.
    Surface renderBuffer;

    // TODO: tiles for optimised rendering
    // 100 24x24 tiles to break up the data sent to the screen, for efficiency.
    Surface tiles[10 * 10];

    // Which tiles have been touched?
    bool touched[10 * 10];

    // Default color
    uint16_t drawColor = TFT_BLACK;

#ifdef RENDER_DMA
    // The extra buffer used for DMA rendering
    Surface dmaBuffer;
#endif // RENDER_DMA

#ifdef OPTIMISED_RENDERING
    // For fast display rendering.
    Arduino_ST7789* tftspi;
#endif // OPTIMISED_RENDERING

    // Whether the display is enabled
    bool enabled = false;

    // How bright the display is
    uint8_t brightness;

};

#endif // DISPLAY_H
