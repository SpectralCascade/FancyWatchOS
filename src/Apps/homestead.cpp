#include "homestead.h"

void Homestead::OnStart(int argc, char* argv[])
{
    timer.Start();
}

void Homestead::Render(Display& display)
{
    display.Clear((timer.GetTicks() % 2000) > 999 ? TFT_GREEN : TFT_RED);
}
