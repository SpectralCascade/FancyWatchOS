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
}
