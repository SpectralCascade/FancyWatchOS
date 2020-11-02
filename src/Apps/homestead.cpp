#include "homestead.h"

void Homestead::OnStart(int argc, char* argv[])
{
    timer.Start();
    // Cache text dimensions
    watch->GetDriver()->tft->setTextSize(3);
    textArea.w = watch->GetDriver()->tft->textWidth("00:00", 4);
    textArea.h = watch->GetDriver()->tft->fontHeight(4);
    textArea.x = 120 - (textArea.w / 2);
    textArea.y = 120 - (textArea.h / 2);
    if (IsForeground())
    {
        watch->GetDriver()->tft->fillScreen(TFT_BLACK);
    }
}

void Homestead::HandleEvent(Event& e)
{
    switch (e.type)
    {
    case EVENT_TOUCH_BEGIN:
        fingers[e.touch.touchID] = true;
    case EVENT_TOUCH_CHANGE:
        touches[e.touch.touchID].DrawFilled(*watch->GetDisplay(), TFT_BLACK);
        touches[e.touch.touchID].x = e.touch.x;
        touches[e.touch.touchID].y = e.touch.y;
        break;
    case EVENT_TOUCH_END:
        touches[e.touch.touchID].DrawFilled(*watch->GetDisplay(), TFT_BLACK);
        fingers[e.touch.touchID] = false;
        break;
    default:
        break;
    }
}

void Homestead::Render(Display& display)
{
    RTC_Date date = watch->GetDriver()->rtc->getDateTime();
    if (lastMinute != date.minute)
    {
        lastMinute = date.minute;

        // Convert date to text
        char text[6] = {'\0'};
        sprintf(text, "%02u:%02u", date.hour, date.minute);

        // Now overwrite previous text
        display.GetTFT()->setTextSize(3);
        textArea.DrawFilled(display, TFT_BLACK);
        display.GetTFT()->drawString(text, textArea.x, textArea.y, 4);
    }

    for (uint8_t i = 0; i < 2; i++)
    {
        if (fingers[i])
        {
            touches[i].DrawFilled(display, i ? TFT_GREEN : TFT_BLUE);
        }
    }
}
