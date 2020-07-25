#ifndef SURFACE_H
#define SURFACE_H

#include "color.h"
#include "utils.h"

enum PixelFormat
{
    PF_RGB565   =   0x0000,
    PF_RGBA4444 =   0x0001,
    PF_RGBA5658 =   0x0002,
    PF_RGBA8888 =   0x0004,
    PF_UNKNOWN  =   0xFFFF
};

// Returns the number of bytes used in a given pixel format.
uint8_t GetDepth(PixelFormat format);

class Surface
{
public:
    // Takes a specified PixelFormat, width, height and color.
    Surface(uint32_t w, uint32_t h, uint8_t format = PF_RGB565, uint32_t color = 0);

    // Makes a copy of a given array of pixels in the specified format.
    Surface(void* pixels, uint32_t w, uint32_t h, uint16_t format = PF_RGB565);

    // Destructor to free pixels
    ~Surface();

    // No implicit copying.
    Surface(const Surface& src) = delete;
    Surface operator=(const Surface& src) = delete;

    // Returns the raw array of pixels.
    void* GetPixels();

    // Returns the number of bytes in a row of the image.
    uint32_t GetPitch();

    // Returns the pixel format.
    uint16_t GetFormat();

    // Returns the width in pixels.
    uint32_t GetWidth();

    // Returns the height in pixels.
    uint32_t GetHeight();

    // Fills the entire surface with a given color.
    void Clear(uint32_t color);

    // TODO: Makes a copy of this surface in the specified format.
    Surface* ConvertTo(uint16_t format);

    // Draws this surface onto another surface at area specified by destRect. If srcRect is NULL, draws the entire surface, otherwise draws pixels from that area.
    void Blit(Surface* dest, IntRect* destRect = nullptr, IntRect* srcRect = nullptr);

private:
    // Internal method for handling scaling if the specified destRect in Blit() has differing dimensions.
    void BlitScaled();

    // Array of pixels
    void* pixels = nullptr;

    // How many pixels make up a single row of the surface
    uint32_t pitch = 0;

    // The format of pixels in this surface
    uint16_t format = PF_RGB565;

    // Width
    uint32_t w;

    // Height
    uint32_t h;

};

#endif // SURFACE_H
