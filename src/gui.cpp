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

//
// Text
//

void Text::Render(Display& display)
{
    if (refresh)
    {
        refresh = false;

        TFT_eSPI* tft = display.GetTFT();
        tft->setTextWrap(wrapText);
        tft->setTextDatum(datum);
        tft->setTextFont(textFont);
        tft->setTextSize(textSize);

        width = tft->textWidth(text.c_str());
        // TODO: account for wrapping.
        height = tft->fontHeight();

        // Clear old text area
        if (oldArea.x != rect.x || oldArea.y != rect.y || oldArea.w != rect.w || oldArea.h != rect.h)
        {
            tft->setTextColor(fg, TFT_TRANSPARENT);
            tft->fillRect(oldArea.x, oldArea.y, oldArea.w, oldArea.h, bg);
            oldArea.x = rect.x;
            oldArea.y = rect.y;
            oldArea.w = width;
            oldArea.h = height;
        }
        else
        {
            // Just overwrite old text.
            tft->setTextColor(fg, bg);
        }

        tft->drawString(text.c_str(), rect.x, rect.y, textFont);
    }
}

void Text::SetText(std::string&& text)
{
    this->text = text;
    // It's up to the caller to be smart about setting text, assume there is a change.
    refresh = true;
}

std::string Text::GetText()
{
    return text;
}

void Text::SetColor(uint16_t color)
{
    refresh |= color != fg;
    fg = color;
}

uint16_t Text::GetColor()
{
    return fg;
}

void Text::SetClearColor(uint16_t color)
{
    refresh |= color != bg;
    bg = color;
}

uint16_t Text::GetClearColor()
{
    return bg;
}

void Text::SetWrap(bool wrap)
{
    refresh |= wrap != wrapText;
    wrapText = wrap;
}

bool Text::IsWrapped()
{
    return wrapText;
}

void Text::SetDatum(uint8_t datum)
{
    refresh |= datum != this->datum;
    this->datum = datum;
}

uint8_t Text::GetDatum()
{
    return datum;
}

void Text::SetSize(uint8_t size)
{
    refresh |= textSize != size;
    textSize = size;
}

uint8_t Text::GetSize()
{
    return textSize;
}

void Text::SetFont(uint8_t selectFont)
{
    refresh |= selectFont != textFont;
    textFont = selectFont;
}

uint8_t Text::GetFont()
{
    return textFont;
}
