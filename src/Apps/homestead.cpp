#include "homestead.h"

void Homestead::OnStart(int argc, char* argv[])
{
    // Setup button
    button = Button(0, 0, 240, 240, SHAPETYPE_INVISIBLE);
    button.strictPress = true;
    button.OnClick = [&] () { this->invert = !this->invert; this->lastMinute--; watch->GetDriver()->tft->fillScreen(invert ? TFT_WHITE : TFT_BLACK); };

    // Cache text dimensions

    // Time text
    watch->GetDriver()->tft->setTextSize(3);
    timeTextArea.w = watch->GetDriver()->tft->textWidth("00:00", 4);
    timeTextArea.h = watch->GetDriver()->tft->fontHeight(4);
    timeTextArea.x = 120 - (timeTextArea.w / 2);
    timeTextArea.y = 120 - (timeTextArea.h / 2);
    if (IsForeground())
    {
        watch->GetDriver()->tft->fillScreen(TFT_BLACK);
        refreshBatteryPercent = true;
    }

    // Battery text
    watch->GetDriver()->tft->setTextSize(1);
    batteryTextArea.w = watch->GetDriver()->tft->textWidth("000%", 4);
    batteryTextArea.h = watch->GetDriver()->tft->fontHeight(4);
    batteryTextArea.x = 120 - (batteryTextArea.w / 2);
    batteryTextArea.y = timeTextArea.y - batteryTextArea.h - 8;
    wipeArea = batteryTextArea;

}

void Homestead::OnEnterBackground()
{
}

void Homestead::OnEnterForeground()
{
    refreshBatteryPercent = true;
}

void Homestead::HandleEvent(Event& e)
{
    if (!watch->GetDisplay()->IsEnabled())
    {
        return;
    }

    switch (e.type)
    {
    case EVENT_POWER_CHARGE:
        charging = true;
        watch->GetDriver()->shake();
        refreshBatteryPercent = true;
        break;
    case EVENT_POWER_DISCONNECT:
        charging = false;
        refreshBatteryPercent = true;
        break;
    case EVENT_POWER_BUTTON:
    case EVENT_POWER_CONNECT:
        refreshBatteryPercent = true;
        break;
    default:
        break;
    }

    button.HandleEvent(e);
}

void Homestead::Render(Display& display)
{
    bool forceRedraw = false;

    // TODO: consider using timer instead of RTC to update every minute, maybe more efficient?
    RTC_Date date = watch->GetDriver()->rtc->getDateTime();
    if (lastMinute != date.minute)
    {
        lastMinute = date.minute;

        // Convert date to text
        char text[6] = {'\0'};
        sprintf(text, "%02u:%02u", date.hour, date.minute);

        // Now overwrite previous text
        display.GetTFT()->setTextSize(3);
        display.GetTFT()->setTextColor(invert ? TFT_BLACK : TFT_WHITE);
        timeTextArea.DrawFilled(display, invert ? TFT_WHITE : TFT_BLACK);
        display.GetTFT()->drawString(text, timeTextArea.x, timeTextArea.y, 4);
        forceRedraw = true;
    }

    if (charging)
    {
        // Show charging symbol
    }

    // Update battery percentage every BATTERY_REFRESH_TIME seconds, or on forced refresh.
    if ((lastSecond != date.second && date.second % BATTERY_REFRESH_TIME == 0) || refreshBatteryPercent || forceRedraw)
    {

        float currentBatteryPercentage = (float)watch->GetDriver()->power->getBattPercentage() / 100.0f;

        // No need to update unless battery percentage actually changes
        if (currentBatteryPercentage != batteryPercentage || forceRedraw)
        {
            batteryPercentage = Clamp(currentBatteryPercentage, 0.0f, 1.0f);
            LogLine(0, "Battery percent = %f", currentBatteryPercentage);

            // Draw text first
            display.GetTFT()->setTextSize(1);
            display.GetTFT()->setTextColor(invert ? TFT_BLACK : TFT_WHITE);

            char text[5] = { '\0', '\0', '\0', '\0', '\0' };
            sprintf(text, "%d%%", (int)(batteryPercentage * 100.0f));

            wipeArea.DrawFilled(display, invert ? TFT_WHITE : TFT_BLACK);
            // Width can change
            batteryTextArea.w = display.GetTFT()->textWidth(text, 4);
            batteryTextArea.x = 120 - (batteryTextArea.w / 2);

            display.GetTFT()->drawString(text, batteryTextArea.x, batteryTextArea.y, 4);

            // Now draw the fancy circle overlay
            // TODO: battery level colour
            display.GetTFT()->drawRoundRect(0, 0, 240, 240, 16, charging ? TFT_GREEN : TFT_BLUE);
            display.GetTFT()->drawRoundRect(1, 1, 238, 238, 16, charging ? TFT_GREEN : TFT_BLUE);

        }

    }
    lastSecond = date.second;
}
