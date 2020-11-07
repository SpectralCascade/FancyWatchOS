#ifndef HOMESTEAD_H
#define HOMESTEAD_H

#include "../app.h"
#include "../coremaths.h"
#include "../gui.h"

#define BATTERY_REFRESH_TIME 10

class Homestead : public Application
{
public:
    void OnStart(int argc, char* argv[]);

    void HandleEvent(Event& e);

    void Render(Display& display);

    void OnEnterBackground();
    void OnEnterForeground();

private:
    Button button;

    bool invert = false;

    bool charging = false;
    bool refreshBatteryPercent = false;

    float batteryPercentage = -1;
    Rect batteryTextArea;
    Rect wipeArea;

    uint8_t lastSecond = 9;
    uint8_t lastMinute = 0;
    Rect timeTextArea;

};

#endif // HOMESTEAD_H
