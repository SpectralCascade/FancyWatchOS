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

    // Setup power monitoring
    driver->power->adc1Enable(AXP202_BATT_VOL_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1, AXP202_ON);

    // Disable unused power outputs.
    /*
        From https://github.com/tuupola/esp_twatch2020

        DC-DC1	PWM charger.
        DC-DC2	Not used 0.7V to 2.275V, 1.6A.
        DC-DC3	ESP32, 0.7V to 3.5V, 1.2A. Always enable!
        LDO1	Always on 30mA.
        LDO2 1)	Display backlight 1.8V to 3.3V, 200mA.
        LDO3 2)	Audio power 0.7V to 3.5V, 200mA.
        LDO4	Not used 1.8V to 3.3V, 200mA.
        LDO5	Not used 1.8V to 3.3V, 50mA.
    */
    driver->power->setPowerOutPut(AXP202_EXTEN, AXP202_OFF);
    driver->power->setPowerOutPut(AXP202_DCDC2, AXP202_OFF);
    driver->power->setPowerOutPut(AXP202_LDO3, AXP202_OFF);
    driver->power->setPowerOutPut(AXP202_LDO4, AXP202_OFF);

    driver->motor_begin();

    // Activate the kernel.
    SetActive(true);

    // Initialise the display settings
    display.SetBrightness(0.5f);
    driver->tft->setTextColor(TFT_WHITE);

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
                EnterSleep();
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

    if (active)
    {
        // Update apps logic. This happens even if an app is not in the foreground, as long as the display is enabled.
        for (unsigned int i = 0; i < totalApps; i++)
        {
            if (apps[i] != nullptr)
            {
                apps[i]->Update();
            }
        }
    }

    if (active)
    {
        // Render foreground apps in reverse order; apps that are added first are then rendered on top.
        for (int i = totalApps - 1; i >= 0; i--)
        {
            if (apps[i] != nullptr && apps[i]->_foreground)
            {
                apps[i]->Render(display);
            }
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

    if (sleepMode)
    {
        sleepMode = false;

        // Set inactive if not already.
        SetActive(false);

        Log("Entering sleep mode...");

        // Start sleeping
        gpio_wakeup_enable((gpio_num_t)AXP202_INT, GPIO_INTR_LOW_LEVEL);
        esp_sleep_enable_gpio_wakeup();
        esp_light_sleep_start();
    }
    else
    {
        // Can safely say the watch was active in this frame.
        wasActive = true;
    }

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

    int32_t toggledEvents = EVENT_TOUCH_BEGIN | EVENT_TOUCH_CHANGE | EVENT_TOUCH_END;

    // Switch between energy-saving and regular operation modes.
    if (active)
    {
        setCpuFrequencyMhz(120);
        display.Enable();
        driver->touchToMonitor();
        EnableEvents(toggledEvents);
    }
    else
    {
        DisableEvents(toggledEvents);
        driver->touchToSleep();
        display.Disable();
        setCpuFrequencyMhz(20);
    }
}

bool Kernel::IsActive()
{
    return active;
}

void Kernel::EnableEvents(int32_t type)
{
    enabledEventsMask |= (int32_t)type;
}

void Kernel::DisableEvents(int32_t type)
{
    enabledEventsMask &= ~((int32_t)type);
}

void Kernel::EnterSleep()
{
    sleepMode = true;
}
