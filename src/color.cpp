#include "color.h"

Color::Color(uint8_t r, uint8_t g, uint8_t b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

Color::Color(uint16_t raw)
{
    *this = (uint16_t)raw;
}
