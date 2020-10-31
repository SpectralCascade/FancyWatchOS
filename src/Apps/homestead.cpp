#include "homestead.h"

int Homestead::Main(int argc, char* argv[])
{
    timer.Start();
    while (1)
    {
        GetRenderer()->Clear((timer.GetTicks() / 1000) % 2 ? TFT_GREEN : TFT_RED);
        GetRenderer()->RenderPresent();
    }
    return 0;
}
