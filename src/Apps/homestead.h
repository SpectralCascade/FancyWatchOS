#ifndef HOMESTEAD_H
#define HOMESTEAD_H

#include "../app.h"
#include "../coremaths.h"
#include "../gui.h"

#define BATTERY_REFRESH_TIME 1

class Homestead : public Application
{
public:
    Homestead() :
        swipeButton(0, 0, 240, 240, SHAPETYPE_INVISIBLE),
        batteryText(120, 120 - 60),
        dateText(120, 120 + 56),
        timeText(120, 120)
#ifdef MONITOR_BATT_TEMP
        , tempText(120, 120 + 56 + 28)
#endif
    {}

    void OnStart(int argc, char* argv[]);

    void HandleEvent(Event& e);

    void Render(Display& display);

    void OnEnterBackground();
    void OnEnterForeground();

private:
    bool wasCharging = false;
    bool charging = false;
    bool refreshBatteryPercent = true;

    Vector2 touchPos = Vector2::Zero;
    Vector2 positionOffset = Vector2::Zero;

    // Button used for screen swiping logic
    Button swipeButton;

    Text batteryText;
    Text dateText;
    Text timeText;
#ifdef MONITOR_BATT_TEMP
    Text tempText;

    float oldTemp = 0;
#endif // MONITOR_BATT_TEMP

    float batteryPercentage = 1.1f;

    bool indicateTouchDebug = false;

    uint8_t lastSecond = 9;
    uint8_t lastMinute = 0;
    uint8_t lastDay = 40;

};

#endif // HOMESTEAD_H
