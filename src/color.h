#ifndef COLOR_H
#define COLOR_H

#include <Arduino.h>

// Convert RGB888 to RGB565 color format, used by the TFT display.
uint16_t Color(uint16_t r, uint16_t g, uint16_t b);

#endif // COLOR_H
