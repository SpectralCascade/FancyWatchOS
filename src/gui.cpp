#include "gui.h"

bool Uint8Rect::Contains(uint8_t px, uint8_t py)
{
    return px < x + w && px >= x && py < y + h && py >= y;
}

Widget::Widget(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
}

void Button::HandleEvent(Event& e)
{
    // Only register one finger.
    if (e.type & (EVENT_TOUCH_BEGIN | EVENT_TOUCH_CHANGE | EVENT_TOUCH_END) && e.touch.touchID == 0)
    {
        switch (e.type)
        {
        case EVENT_TOUCH_CHANGE:
            if (strictPress)
            {
                break;
            }
        case EVENT_TOUCH_BEGIN:
            pressed = rect.Contains(e.touch.x, e.touch.y);
            break;
        case EVENT_TOUCH_END:
            if (pressed)
            {
                pressed = false;
                if (rect.Contains(e.touch.x, e.touch.y))
                {
                    OnClick();
                }
            }
            break;
        default:
            break;
        }
    }
}

void Button::Render(Display& display)
{
    if (wasPressed != pressed && shape != SHAPETYPE_INVISIBLE)
    {
        switch (shape)
        {
        case SHAPETYPE_RECT:
            display.GetTFT()->fillRect(rect.x, rect.y, rect.w, rect.h, pressed ? colorPressed : colorNormal);
            break;
        case SHAPETYPE_RECT_ROUNDED:
            display.GetTFT()->fillRoundRect(rect.x - radius, rect.y - radius, rect.w, rect.h, radius, pressed ? colorPressed : colorNormal);
            break;
        case SHAPETYPE_CIRCLE:
            display.GetTFT()->fillCircle(rect.x, rect.y, radius, pressed ? colorPressed : colorNormal);
            break;
        case SHAPETYPE_ELLIPSE:
            display.GetTFT()->fillEllipse(rect.x, rect.y, rect.w, rect.h, pressed ? colorPressed : colorNormal);
            break;
        default:
            break;
        }
        wasPressed = pressed;
    }
}

bool Button::IsPressed()
{
    return pressed;
}
