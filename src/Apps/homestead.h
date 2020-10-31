#ifndef HOMESTEAD_H
#define HOMESTEAD_H

#include "../app.h"

class Homestead : public Application
{
public:
    int Main(int argc, char* argv[]);

private:
    Timer timer;

};

#endif // HOMESTEAD_H
