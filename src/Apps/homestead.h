#ifndef HOMESTEAD_H
#define HOMESTEAD_H

#include "../app.h"
#include "../coremaths.h"

class Homestead : public Application
{
public:
    void OnStart(int argc, char* argv[]);

    void Render(Display& display);

private:
    Timer timer;

    uint8_t lastMinute = 0;
    Rect textArea;

};

#endif // HOMESTEAD_H
