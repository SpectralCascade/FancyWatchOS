#ifndef HOMESTEAD_H
#define HOMESTEAD_H

#include "../app.h"
#include "../coremaths.h"
#include "../gui.h"

#define BATTERY_REFRESH_TIME 10

class Homestead : public Application
{
public:
    Homestead() : batteryText(120, 120 - 56), dateText(120, 120 + 56), timeText(120, 120) {}

    void OnStart(int argc, char* argv[]);

    void HandleEvent(Event& e);

    void Render(Display& display);

    void OnEnterBackground();
    void OnEnterForeground();

private:
    bool wasCharging = false;
    bool charging = false;
    bool refreshBatteryPercent = true;

    Text batteryText;
    Text dateText;
    Text timeText;

    float batteryPercentage = 1.1f;

    uint8_t lastSecond = 9;
    uint8_t lastMinute = 0;
    uint8_t lastDay = 40;

};

#endif // HOMESTEAD_H
