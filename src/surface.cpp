#include <math.h>
#include "surface.h"

uint8_t GetDepth(PixelFormat format)
{
    switch (format)
    {
    case PF_RGB565:
    case PF_RGBA4444:
        return 2;
    case PF_RGBA5658:
        return 3;
    case PF_RGBA8888:
    default:
        return 4;
    }
}

Surface::Surface(uint32_t w, uint32_t h, uint8_t format, uint32_t color)
{
    this->format = format;
    uint32_t depth = GetDepth((PixelFormat)format);
    pitch = depth * w;
    // TODO: support other formats
    //pixels = new uint16_t[(pitch / 2) * h] { 0 };
}

Surface::Surface(void* pixels, uint32_t w, uint32_t h, uint16_t format)
{
    this->w = w;
    this->h = h;
    this->format = format;

    pitch = ((uint32_t)GetDepth((PixelFormat)format)) * w;

    uint32_t counti = pitch * h;
    this->pixels = new uint8_t[counti];
    for (uint32_t i = 0; i < counti; i++)
    {
        ((uint8_t*)this->pixels)[i] = ((uint8_t*)pixels)[i];
    }
}

Surface::~Surface()
{
    // TODO: handle different formats
    //delete[] (uint16_t*)pixels;
}

void* Surface::GetPixels()
{
    return pixels;
}

uint32_t Surface::GetPitch()
{
    return pitch;
}

uint16_t Surface::GetFormat()
{
    return format;
}

uint32_t Surface::GetWidth()
{
    return w;
}

uint32_t Surface::GetHeight()
{
    return h;
}

Surface* Surface::ConvertTo(uint16_t format)
{
    Surface* created = nullptr;

    created = new Surface(pixels, pitch, format);
    // TODO: implement support for different formats
    /*if (this->format != format)
    {
        void* converted = new
        created = new Surface();
    }
    else
    {
        created = new Surface(pixels, pitch, format);
    }*/
    return created;
}

void Surface::Clear(uint32_t color)
{
    uint8_t depth = GetDepth((PixelFormat)format);
    switch (depth)
    {
    default:
        // Default to 16-bit.
        // TODO: support other formats
        for (uint32_t i = 0, counti = w * h; i < counti; i++)
        {
            ((uint16_t*)pixels)[i] = (uint16_t)color;
        }
        break;
    }
}

void Surface::Blit(Surface* dest, IntRect* destRect, IntRect* srcRect)
{
    bool didConvert = false;
    Surface* converted = this;
    if (format != dest->format)
    {
        // First, make sure the surfaces are the same format
        converted = ConvertTo(dest->format);
        didConvert = true;
    }

    // Now both are the same format, locate the starting position in the destination surface for each row and blit across.
    uint32_t depth = GetDepth((PixelFormat)format);

    uint32_t step = ((uint32_t)max(destRect->x, 0)) * depth;
    for (uint32_t i = (uint32_t)(destRect->y * pitch) + step; i < pitch * dest->w * dest->h; i += step)
    {
        //uint32_t stopIndex = (uint32_t)(destRect->x * depth)
    }

    if (didConvert)
    {
        // Once done, we can free the converted surface copy.
        delete converted;
    }
}
