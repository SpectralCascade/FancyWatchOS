#include "renderer.h"
#include "kernel.h"

Renderer::Renderer(Kernel* kernel)
{
    this->kernel = kernel;
    // Use built in RAM by default as it's fastest. If pixels did not allocate, try again in PSRAM
    Init(240, 240, PF_RGB565, false);
    if (pixels == NULL)
    {
        Init(240, 240, PF_RGB565, true);
    }
}

Renderer::~Renderer()
{
    Destroy();
}

void Renderer::RenderPresent()
{
    frameTimer.Start();
    kernel->RunSystemTask(
        // Copy to the display buffer directly for time being.
        [&] () { kernel->display.GetBuffer()->Replicate(this); }
    );
    // Now wait for the rest of the frame to ensure the user doesn't repeatedly render unnecessarily.
    uint32_t waitTime = frameTimer.GetTicks() - DISPLAY_REFRESH_DELAY;
    vTaskDelay((TickType_t)waitTime);
}
