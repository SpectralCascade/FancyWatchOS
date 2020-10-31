#ifndef RENDERER_H
#define RENDERER_H

#include "surface.h"

class Kernel;

// A renderer instance that abstracts away the display and can exist per-application.
// You cannot copy it.
class Renderer : public Surface
{
public:
    Renderer(Kernel* kernel);
    ~Renderer();

    // Copies the surface to the display via the Kernel.
    void RenderPresent();

private:
    friend class Kernel;

    Kernel* kernel;

    // No copying
    Renderer(const Renderer& renderer) = delete;
    Renderer& operator=(const Renderer& renderer) = delete;

    // When RenderPresent() is called, used to wait until the end of the frame.
    Timer frameTimer;

};

#endif // RENDERER_H
