#include "config.h"
#include "utils.h"
#include "display.h"
#include "app.h"
#include "Apps/homestead.h"

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

    LogMark(__FILE__, __LINE__);

    // Start up with the default app.
    StartApp(new Homestead());

}

Kernel::~Kernel()
{
    display.Destroy();
}

void Kernel::Update()
{
    LogMark(__FILE__, __LINE__);

    // Should the watch deep sleep at the end of this update?
    bool deepSleep = false;

    // Check for system-level input events
    Event e;
    while (uxQueueMessagesWaiting(events) > 0)
    {
        xQueueReceive(events, &e, portMAX_DELAY);
        switch (e.type)
        {
        case EVENT_POWER_BUTTON:
            if (display.IsEnabled())
            {
                display.Disable();
            }
            else
            {
                display.Enable();
            }
            break;
        default:
            break;
        }
    }

    if (display.IsEnabled() && renderTimer.GetTicks() >= DISPLAY_REFRESH_DELAY)
    {
        SuspendAllApps();

        //display.Clear(TFT_BLUE);
        // Refresh the screen.
        display.RenderPresent();

        ResumeAllApps();

        // Restart the timer.
        renderTimer.Start();
    }

    if (deepSleep)
    {
        display.Disable();

        esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);

        esp_deep_sleep_start();
    }
}

void Kernel::RunSystemTask(std::function<void()> task)
{
    // Stop the scheduler so only this thread runs
    SuspendAllApps();

    // Run the task, whatever that may be.
    task();

    // Resume the scheduler
    ResumeAllApps();
}

int Kernel::StartApp(Application* app, bool foreground, int argc, char* argv[])
{
    int id = -1;
    if (app != nullptr)
    {
        // Make sure no apps are modified on other threads while we add this one.
        SuspendAllApps();
        for (int i = 0; i < MAX_APPS; i++)
        {
            if (taskHandles[i] == nullptr)
            {
                Log("Starting application[%d]...\n", i);

                // Set ID to match the task handle index.
                id = i;

                // Generate task name, just set to the id for now.
                char name[5] = {'\0'};
                sprintf(name, "%d", id);

                // Copy command arguments
                apps[i] = app;
                if (app->_argv != nullptr)
                {
                    for (uint32_t j = 0; j < app->_argc; j++)
                    {
                        delete[] app->_argv[j];
                    }
                    delete[] app->_argv;
                }
                app->_argc = argc;
                app->_argv = new char*[argc];
                app->_id = id;

                for (uint32_t j = 0; j < argc; j++)
                {
                    app->_argv[j] = new char[strlen(argv[j]) + 1];
                    strcpy(app->_argv[j], argv[j]);
                }
                app->Init(this);

                // Now create the actual task for the app.
                xTaskCreate(
                    [] (void* task) {
                        ((Application*)task)->Main(((Application*)task)->_argc, ((Application*)task)->_argv);
                        ((Application*)task)->Cleanup();
                    },
                    name,
                    MAX_APP_STACK,
                    (void*)app,
                    tskIDLE_PRIORITY,
                    &taskHandles[i]
                );

                if (taskHandles[i])
                {
                    Log("Application[%d] started.\n", i);
                }
                else
                {
                    LogError("Failed to start task for application[%d]!\n", i);
                }

                break;
            }
        }
        ResumeAllApps();
    }
    return id;
}

Application* Kernel::KillApp(int id, bool force)
{
    Application* app = nullptr;
    SuspendAllApps();
    if (id >= 0 && id < MAX_APPS && taskHandles[id] != nullptr)
    {
        if (!force)
        {
            apps[id]->OnStop();
        }
        vTaskDelete(taskHandles[id]);
        taskHandles[id] = nullptr;
        // Note: killing an app doesn't actually destroy it.
        app = apps[id];
        apps[id] = nullptr;
    }
    ResumeAllApps();
    return app;
}

Display* Kernel::GetDisplay()
{
    return &display;
}

TTGOClass* Kernel::GetDriver()
{
    return driver;
}

void Kernel::SuspendAllApps()
{
    for (uint32_t i = 0; i < MAX_APPS; i++)
    {
        if (taskHandles[i] != nullptr)
        {
            vTaskSuspend(taskHandles[i]);
        }
    }
}

void Kernel::ResumeAllApps()
{
    for (uint32_t i = 0; i < MAX_APPS; i++)
    {
        if (taskHandles[i] != nullptr)
        {
            vTaskResume(taskHandles[i]);
        }
    }
}
