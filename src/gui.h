#ifndef GUI_H
#define GUI_H

#include "coremaths.h"
#include "kernel.h"

class Widget
{
public:
    Vector2 pos = Vector2::Zero;

    // Used for relative positions.
    Widget* parent = nullptr;

};

class Button : public Widget
{
public:
    void HandleEvent(Event& e);

    void Render(Display& display);

};

#endif // GUI_H
