#ifndef APP_H
#define APP_H

#include "kernel.h"

class Application
{
public:
    friend class Kernel;

    // Clean up command arguments.
    virtual ~Application();

    // Called when the app is killed.
    virtual void OnStop();

    // Called when the app should run in the foreground.
    virtual void OnEnterForeground();

    // Called when the app should run in the background.
    virtual void OnEnterBackground();

    // The app Main() function. Must be implemented.
    virtual int Main(int argc, char* argv[]) = 0;

    // Is this app running in the foreground?
    bool IsForeground();

protected:
    // Returns the renderer instance, or creates one if it doesn't exist already.
    Renderer* GetRenderer();

    // Reference to the watch runtime itself.
    Kernel* watch;

private:
    void Init(Kernel* kernel);

    void Cleanup();

    // Main arguments
    int _argc = 0;
    char** _argv = nullptr;

    // This application's runtime task ID.
    int _id = -1;

    // The one and only renderer associated with this application.
    Renderer* _renderer = nullptr;

    // Returned by IsForeground().
    bool _foreground = false;

};

#endif // APP_H
