#include "color.h"

uint16_t Color(uint16_t r, uint16_t g, uint16_t b)
{
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

