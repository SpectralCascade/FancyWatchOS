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

    void Render(Display& display, const Vector2& offset = Vector2::Zero);

    uint16_t colorNormal = COLOR(0, 200, 200);
    uint16_t colorPressed = COLOR(0, 100, 100);

    bool IsPressed();

    std::function<void()> OnClick = [] () {};
    std::function<void(const Vector2&)> OnPointerDown = [] (const Vector2& pos) {};
    std::function<void(const Vector2&)> OnPointerUp = [] (const Vector2& pos) {};
    std::function<void(const Vector2&)> OnDrag = [] (const Vector2& pos) {};

    // Set to true to ensure that presses are only registered upon EVENT_TOUCH_BEGIN.
    bool strictPress = false;

    uint8_t radius = 4;

    ShapeType shape = SHAPETYPE_RECT;

private:
    bool pressed = false;
    bool wasPressed = true;

};

class Text : public Widget
{
public:
    Text() { oldArea.x = oldArea.y = oldArea.w = oldArea.h = 0; }
    Text(uint8_t x, uint8_t y, uint8_t maxWidth = 240, uint8_t maxHeight = 240) : Widget(x, y, maxWidth, maxHeight) { oldArea.x = oldArea.y = oldArea.w = oldArea.h = 0; }

    // Renders the text if it has changed.
    void Render(Display& display, const Vector2& offset = Vector2::Zero);

    // Set the text string.
    void SetText(const char* text);

    // Return the text string.
    const char* GetText();

    // Set the text color
    void SetColor(uint16_t color);

    // Get the text color
    uint16_t GetColor();

    // Set the text background color, used to overwrite the old text.
    void SetClearColor(uint16_t color);

    // Return the text background color, used to overwrite the old text.
    uint16_t GetClearColor();

    // Set text wrapping.
    void SetWrap(bool wrap);

    // Is the text wrapping
    bool IsWrapped();

    // Set text datum.
    void SetDatum(uint8_t datum);

    // Return the text datum.
    uint8_t GetDatum();

    // Set the preset font size in increments.
    void SetSize(uint8_t size);

    // Return the font size in increments.
    uint8_t GetSize();

    // Set the font to use.
    void SetFont(uint8_t selectFont);

    // Return the font used.
    uint8_t GetFont();

    // Outputs the anchor/datum position into x and y given a text width.
    void GetDatumOffset(uint8_t width, uint8_t height, uint8_t* x, uint8_t* y);

private:
    // The old area the text was drawn in.
    Uint8Rect oldArea;

    // The text itself.
    std::string text;

    // Width of the text.
    uint8_t width = 240;

    // Height of the text.
    uint8_t height = 240;

    // The current font to use.
    uint8_t textFont = 6;

    // The preset size of the text.
    uint8_t textSize = 3;

    // Should the text be re-drawn on the next render call?
    bool refresh = false;

    // Should this text wrap?
    bool wrapText = false;

    // See TFT_eSPI.h Section 5 for info. Basically how the text is anchored.
    // Defaults to top left.
    uint8_t datum = TL_DATUM;

    // Text color
    uint16_t fg = TFT_WHITE;

    // Clear color
    uint16_t bg = TFT_BLACK;

};

#endif // GUI_H
