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
            {
                if (pressed)
                {
                    OnDrag(Vector2((float)e.touch.x, (float)e.touch.y));
                }
                if (strictPress)
                {
                    break;
                }
            }
            case EVENT_TOUCH_BEGIN:
            {
                pressed = rect.Contains(e.touch.x, e.touch.y);
                if (pressed && !wasPressed)
                {
                    OnPointerDown(Vector2((float)e.touch.x, (float)e.touch.y));
                }
                break;
            }
            case EVENT_TOUCH_END:
            {
                if (pressed)
                {
                    pressed = false;
                    OnPointerUp(Vector2((float)e.touch.x, (float)e.touch.y));
                    if (rect.Contains(e.touch.x, e.touch.y))
                    {
                        OnClick();
                    }
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

void Button::Render(Display& display, const Vector2& offset)
{
    if (wasPressed != pressed && shape != SHAPETYPE_INVISIBLE)
    {
        switch (shape)
        {
        case SHAPETYPE_RECT:
            display.GetTFT()->fillRect((int32_t)rect.x + (int32_t)offset.x, (int32_t)rect.y + (int32_t)offset.y, rect.w, rect.h, pressed ? colorPressed : colorNormal);
            break;
        case SHAPETYPE_RECT_ROUNDED:
            display.GetTFT()->fillRoundRect(((int32_t)rect.x + (int32_t)offset.x) - radius, ((int32_t)rect.y + (int32_t)offset.y) - radius, rect.w, rect.h, radius, pressed ? colorPressed : colorNormal);
            break;
        case SHAPETYPE_CIRCLE:
            display.GetTFT()->fillCircle((int32_t)rect.x + (int32_t)offset.x, (int32_t)rect.y + (int32_t)offset.y, radius, pressed ? colorPressed : colorNormal);
            break;
        case SHAPETYPE_ELLIPSE:
            display.GetTFT()->fillEllipse((int32_t)rect.x + (int32_t)offset.x, (int32_t)rect.y + (int32_t)offset.y, rect.w, rect.h, pressed ? colorPressed : colorNormal);
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

void Text::Render(Display& display, const Vector2& offset)
{
    if (refresh)
    {
        refresh = false;

        TFT_eSPI* tft = display.GetTFT();
        tft->setTextWrap(wrapText);
        tft->setTextDatum(datum);
        tft->setTextFont(textFont);
        tft->setTextSize(textSize);
        tft->setTextColor(fg);

        width = tft->textWidth(text.c_str());
        // TODO: account for wrapping.
        height = tft->fontHeight();

        // Clear old text area
        tft->fillRect((int32_t)oldArea.x + (int32_t)offset.x, (int32_t)oldArea.y + (int32_t)offset.y, oldArea.w > 0 ? oldArea.w : width, oldArea.h > 0 ? oldArea.h : height, bg);

        uint8_t x, y;
        GetDatumOffset(width, height, &x, &y);

        // True position is based on the datum.
        oldArea.x = rect.x - x;
        oldArea.y = rect.y - y;
        oldArea.w = width;
        oldArea.h = height;

        tft->drawString(text.c_str(), (int32_t)rect.x + (int32_t)offset.x, (int32_t)rect.y + (int32_t)offset.x, textFont);
    }
}

void Text::SetText(const char* text)
{
    this->text = text;
    // It's up to the caller to be smart about setting text, assume there is a change.
    refresh = true;
}

const char* Text::GetText()
{
    return text.c_str();
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

void Text::GetDatumOffset(uint8_t width, uint8_t height, uint8_t* x, uint8_t* y)
{
    /**
        // These enumerate the text plotting alignment (reference datum point)
        #define TL_DATUM 0 // Top left (default)
        #define TC_DATUM 1 // Top centre
        #define TR_DATUM 2 // Top right
        #define ML_DATUM 3 // Middle left
        #define CL_DATUM 3 // Centre left, same as above
        #define MC_DATUM 4 // Middle centre
        #define CC_DATUM 4 // Centre centre, same as above
        #define MR_DATUM 5 // Middle right
        #define CR_DATUM 5 // Centre right, same as above
        #define BL_DATUM 6 // Bottom left
        #define BC_DATUM 7 // Bottom centre
        #define BR_DATUM 8 // Bottom right

        // These aren't supported...
        #define L_BASELINE  9 // Left character baseline (Line the 'A' character would sit on)
        #define C_BASELINE 10 // Centre character baseline
        #define R_BASELINE 11 // Right character baseline
    */
    switch (datum)
    {
    default:
    case TL_DATUM:
        *x = 0;
        *y = 0;
        break;
    case TC_DATUM:
        *x = width / 2;
        *y = 0;
        break;
    case TR_DATUM:
        *x = width;
        *y = 0;
        break;
    case ML_DATUM:
        *x = 0;
        *y = height / 2;
        break;
    case MC_DATUM:
        *x = width / 2;
        *y = height / 2;
        break;
    case MR_DATUM:
        *x = width;
        *y = height / 2;
        break;
    case BL_DATUM:
        *x = 0;
        *y = height;
        break;
    case BC_DATUM:
        *x = width / 2;
        *y = height;
        break;
    case BR_DATUM:
        *x = width;
        *y = height;
        break;
    }
}
