#ifndef GUI_H
#define GUI_H

#include "coremaths.h"
#include "kernel.h"
#include <functional>

class Uint8Rect
{
public:
    bool Contains(uint8_t px, uint8_t py);

    uint8_t x;
    uint8_t y;
    uint8_t w;
    uint8_t h;
};

class Widget
{
public:
    Widget() = default;
    Widget(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

    Uint8Rect rect;

    // Used for relative positions.
    Widget* parent = nullptr;

};

enum ShapeType
{
    SHAPETYPE_INVISIBLE = 0,
    SHAPETYPE_RECT,
    SHAPETYPE_RECT_ROUNDED,
    SHAPETYPE_CIRCLE,
    SHAPETYPE_ELLIPSE,
};

class Button : public Widget
{
public:
    Button() = default;
    Button(uint8_t x, uint8_t y, uint8_t w, uint8_t h, ShapeType shapeType = SHAPETYPE_RECT_ROUNDED) : Widget(x, y, w, h), shape(shapeType) {}

    void HandleEvent(Event& e);

    void Render(Display& display);

    uint16_t colorNormal = COLOR(0, 200, 200);
    uint16_t colorPressed = COLOR(0, 100, 100);

    bool IsPressed();

    std::function<void()> OnClick = [] () {};

    // Set to true to ensure that presses are only registered upon EVENT_TOUCH_BEGIN.
    bool strictPress = false;

    uint8_t radius = 4;

    ShapeType shape = SHAPETYPE_RECT;

private:
    bool pressed = false;
    bool wasPressed = true;

};

#endif // GUI_H
