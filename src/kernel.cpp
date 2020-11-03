#include "config.h"
#include "utils.h"
#include "display.h"
#include "app.h"

SemaphoreHandle_t gSystemMutex = nullptr;

Kernel::Kernel(TTGOClass* device, QueueHandle_t eventQueue)
{
    // Initialise the watch
    driver = device;
    driver->begin();
    // Setup display
    events = eventQueue;
    display.Init(driver);

    renderTimer.Start();
}

Kernel::~Kernel()
{
    display.Destroy();
}

void Kernel::Update()
{
    // Check for system-level input events
    Event e;
    while (uxQueueMessagesWaiting(events) > 0)
    {
        xQueueReceive(events, &e, portMAX_DELAY);
        switch (e.type)
        {
        case EVENT_POWER_BUTTON:
            if (wasActive)
            {
                SetActive(false);
            }
            break;
        default:
            break;
        }

        // Now apps can handle the event. This happens even if an app is not in the foreground.
        for (unsigned int i = 0; i < totalApps; i++)
        {
            if (apps[i] != nullptr)
            {
                apps[i]->HandleEvent(e);
            }
        }
    }

    if (!active)
    {
        // Early out if set inactive by event handling
        return;
    }

    // Update apps logic. This happens even if an app is not in the foreground, as long as the display is enabled.
    for (unsigned int i = 0; i < totalApps; i++)
    {
        if (apps[i] != nullptr)
        {
            apps[i]->Update();
        }
    }

    if (!active)
    {
        // Early out if set inactive by app updates
        return;
    }

    // Render foreground apps in reverse order; apps that are added first are then rendered on top.
    for (int i = totalApps - 1; i >= 0; i--)
    {
        if (apps[i] != nullptr && apps[i]->_foreground)
        {
            apps[i]->Render(display);
        }
    }

    // Always delay to save some processing time, no matter if the display is active.
    uint32_t frameWaitTime = DISPLAY_REFRESH_DELAY - renderTimer.GetTicks();
    if (frameWaitTime <= DISPLAY_REFRESH_DELAY)
    {
        // Now delay until the next refresh
        vTaskDelay(frameWaitTime);
    }

    // Restart the timer.
    renderTimer.Start();

    if (deepSleep)
    {
        display.Disable();

        esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);

        esp_deep_sleep_start();
    }

    // Can safely say the watch was active in this frame.
    wasActive = true;

}

int Kernel::StartApp(Application* app, bool foreground, int argc, char* argv[])
{
    int id = -1;
    if (app != nullptr)
    {
        if (totalApps < MAX_APPS)
        {
            // Set ID to match the task handle index.
            id = totalApps;
            apps[id] = app;
            totalApps++;

            Log("Starting application[%d]...\n", id);
            app->_foreground = foreground;
            app->watch = this;
            app->OnStart(argc, argv);
        }

        if (id < 0)
        {
            LogError("Failed to start application! The maximum number of applications are running already.");
        }

    }
    return id;
}

Application* Kernel::KillApp(int id, bool force)
{
    Application* app = nullptr;
    if (id >= 0 && id < MAX_APPS && apps[id] != nullptr)
    {
        if (!force)
        {
            apps[id]->OnStop();
        }
        app = apps[id];
        // Collapse the array for better performance.
        for (unsigned int i = id + 1; i < totalApps; i++)
        {
            apps[i - 1] = apps[i];
        }
        totalApps--;
    }
    // Note: killing an app doesn't actually destroy it, hence we return it when done.
    return app;
}

void Kernel::SetActive(bool active)
{
    if (!active)
    {
        if (wasActive)
        {
            wasActive = false;
        }
        else
        {
            // Early out, shouldn't set inactive on first active frame.
            return;
        }
    }

    this->active = active;

    // Setup peripherals
    if (active != display.IsEnabled())
    {
        active ? display.Enable() : display.Disable();
    }
}

bool Kernel::IsActive()
{
    return active;
}

void Kernel::DeepSleep()
{
    deepSleep = true;
}

Display* Kernel::GetDisplay()
{
    return &display;
}

TTGOClass* Kernel::GetDriver()
{
    return driver;
}
