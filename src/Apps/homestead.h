#ifndef HOMESTEAD_H
#define HOMESTEAD_H

#include "../app.h"

class Homestead : public Application
{
public:
    void OnStart(int argc, char* argv[]);

    void Render(Display& display);

private:
    Timer timer;

};

#endif // HOMESTEAD_H
