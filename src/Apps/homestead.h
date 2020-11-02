#ifndef HOMESTEAD_H
#define HOMESTEAD_H

#include "../app.h"
#include "../coremaths.h"

class Homestead : public Application
{
public:
    void OnStart(int argc, char* argv[]);

    void HandleEvent(Event& e);

    void Render(Display& display);

private:
    Timer timer;

    uint8_t lastMinute = 0;
    Rect textArea;

    Circle touches[2] = { { -1, -1, 32 }, { -1, -1, 32 } };
    bool fingers[2] = {false};

};

#endif // HOMESTEAD_H
