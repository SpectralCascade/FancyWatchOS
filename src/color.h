#ifndef COLOR_H
#define COLOR_H

#include <Arduino.h>

// RGB565 color format, used by the TFT display. Has implicit conversion to and from uint16_t.
struct Color
{
    Color() = default;
    Color(uint16_t raw);
    Color(uint8_t r, uint8_t g, uint8_t b);

    operator uint16_t() const { return (uint8_t)*this; }

    // 16-bit RGB565 format
    uint8_t r : 5;
    uint8_t g : 6;
    uint8_t b : 5;
};

#endif // COLOR_H
