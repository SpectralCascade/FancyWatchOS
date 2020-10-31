#include "app.h"

Application::~Application()
{
    if (_argv != nullptr)
    {
        for (uint32_t j = 0; j < _argc; j++)
        {
            delete[] _argv[j];
        }
        delete[] _argv;
    }
    _argc = 0;
}

void Application::OnStop()
{
}

void Application::OnEnterForeground()
{
}

void Application::OnEnterBackground()
{
}

bool Application::IsForeground()
{
    return _foreground;
}

void Application::Init(Kernel* kernel)
{
    watch = kernel;
}

Renderer* Application::GetRenderer()
{
    if (_renderer == nullptr)
    {
        _renderer = new Renderer(watch);
    }
    return _renderer;
}
