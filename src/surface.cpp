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

void Surface::Init(uint32_t w, uint32_t h, uint8_t format, bool usePSRAM)
{
    this->format = format;
    uint32_t depth = GetDepth((PixelFormat)format);
    pitch = depth * w;
    pixels = usePSRAM ? ps_malloc(pitch * h) : malloc(pitch * h);
    if (pixels == NULL)
    {
        LogError("Failed to allocate memory for surface!\n");
    }
    else
    {
        this->w = w;
        this->h = h;
    }
}

void Surface::Destroy()
{
    free(pixels);
}

void* BaseSurface::GetPixels()
{
    return pixels;
}

uint32_t BaseSurface::GetPitch()
{
    return pitch;
}

uint16_t BaseSurface::GetFormat()
{
    return format;
}

uint32_t BaseSurface::GetWidth()
{
    return w;
}

uint32_t BaseSurface::GetHeight()
{
    return h;
}

/*
Surface* Surface::ConvertTo(uint16_t format)
{
    Surface* created = nullptr;

    created = new Surface(pixels, pitch, format);
    // TODO: implement support for different formats
    if (this->format != format)
    {
        void* converted = new
        created = new Surface();
    }
    else
    {
        created = new Surface(pixels, pitch, format);
    }
    return created;
}
*/

void BaseSurface::Clear(uint32_t color)
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
/*
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
*/

