#ifndef APP_H
#define APP_H

#include "kernel.h"

class Application
{
public:
    friend class Kernel;

    // Called when the app is initialised.
    virtual void OnStart(int argc, char* argv[]);

    // Called when the app is killed.
    virtual void OnStop();

    // Called when the app should run in the foreground.
    virtual void OnEnterForeground();

    // Called when the app should run in the background.
    virtual void OnEnterBackground();

    // Handle an input event.
    virtual void HandleEvent(Event& e);

    // Update logic once per frame.
    virtual void Update();

    // Render stuff to the display.
    virtual void Render(Display& display);

    // Is this app running in the foreground?
    bool IsForeground();

protected:
    // Reference to the watch runtime itself.
    Kernel* watch;

private:
    // This application's runtime task ID.
    int _id = -1;

    // Returned by IsForeground().
    bool _foreground = false;

};

#endif // APP_H
