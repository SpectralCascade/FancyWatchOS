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
    driver->power->adc1Enable(AXP202_TS_PIN_ADC1 | AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, true);
    driver->power->adc2Enable(AXP202_TEMP_MONITORING_ADC2, true);

    driver->motor_begin();

    // Setup the BMA accelerometer
    Acfg config;
    /**
        Output data rate in Hz, Optional parameters:
            - BMA4_OUTPUT_DATA_RATE_0_78HZ
            - BMA4_OUTPUT_DATA_RATE_1_56HZ
            - BMA4_OUTPUT_DATA_RATE_3_12HZ
            - BMA4_OUTPUT_DATA_RATE_6_25HZ
            - BMA4_OUTPUT_DATA_RATE_12_5HZ
            - BMA4_OUTPUT_DATA_RATE_25HZ
            - BMA4_OUTPUT_DATA_RATE_50HZ
            - BMA4_OUTPUT_DATA_RATE_100HZ
            - BMA4_OUTPUT_DATA_RATE_200HZ
            - BMA4_OUTPUT_DATA_RATE_400HZ
            - BMA4_OUTPUT_DATA_RATE_800HZ
            - BMA4_OUTPUT_DATA_RATE_1600HZ
    */
    config.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
    /**
        G-range, Optional parameters:
            - BMA4_ACCEL_RANGE_2G
            - BMA4_ACCEL_RANGE_4G
            - BMA4_ACCEL_RANGE_8G
            - BMA4_ACCEL_RANGE_16G
    */
    config.range = BMA4_ACCEL_RANGE_2G;
    /**
        Bandwidth parameter, determines filter configuration, Optional parameters:
            - BMA4_ACCEL_OSR4_AVG1
            - BMA4_ACCEL_OSR2_AVG2
            - BMA4_ACCEL_NORMAL_AVG4
            - BMA4_ACCEL_CIC_AVG8
            - BMA4_ACCEL_RES_AVG16
            - BMA4_ACCEL_RES_AVG32
            - BMA4_ACCEL_RES_AVG64
            - BMA4_ACCEL_RES_AVG128
    */
    config.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
    /** Filter performance mode , Optional parameters:
        - BMA4_CIC_AVG_MODE
        - BMA4_CONTINUOUS_MODE
    */
    config.perf_mode = BMA4_CONTINUOUS_MODE;

    // Apply configuration to the sensor.
    driver->bma->accelConfig(config);

    // Disable BMA423 step counting (for now).
    driver->bma->enableFeature(BMA423_STEP_CNTR, false);
    // Keep tilt wakeup enabled, it's the most useful.
    driver->bma->enableFeature(BMA423_TILT, true);
    // Disable the double tap, it's not that useful.
    driver->bma->enableFeature(BMA423_WAKEUP, false);

    // Set step count to zero.
    driver->bma->resetStepCounter();

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
    bool eventOccurred = false;
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
        // TODO: Only update napTimer for events that trigger a wakeup.
        eventOccurred = true;
    }

    if (eventOccurred)
    {
        napTimer.Start();
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

    if (sleepMode || napTimer.GetTicks() > DISPLAY_TIMEOUT)
    {
        sleepMode = false;
        napTimer.Stop();

        // Set inactive if not already.
        SetActive(false);

        Log("Entering sleep mode...");

        // First setup the power interrupts.
        gpio_wakeup_enable((gpio_num_t)AXP202_INT, GPIO_INTR_LOW_LEVEL);
        // Then the BMA interrupts.
        esp_sleep_enable_ext1_wakeup(GPIO_SEL_39, ESP_EXT1_WAKEUP_ANY_HIGH);
        esp_sleep_enable_gpio_wakeup();

        // Start sleeping
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
        napTimer.Start();
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
