#include "homestead.h"

void Homestead::OnStart(int argc, char* argv[])
{
    // Cache text dimensions

    // Time text
    watch->driver->tft->setTextSize(3);
    timeTextArea.w = watch->driver->tft->textWidth("00:00", 4);
    timeTextArea.h = watch->driver->tft->fontHeight(4);
    timeTextArea.x = 120 - (timeTextArea.w / 2);
    timeTextArea.y = 120 - (timeTextArea.h / 2);
    if (IsForeground())
    {
        watch->driver->tft->fillScreen(TFT_BLACK);
    }

    // Date text
    watch->driver->tft->setTextSize(1);
    dateTextArea.w = watch->driver->tft->textWidth("Sun 13th Feb", 4);
    dateTextArea.h = watch->driver->tft->fontHeight(4);
    dateTextArea.x = 120 - (dateTextArea.w / 2);
    dateTextArea.y = timeTextArea.y + timeTextArea.h + 8;
    if (IsForeground())
    {
        watch->driver->tft->fillScreen(TFT_BLACK);
    }
    wipeDateArea = dateTextArea;

    // Battery text
    batteryTextArea.w = watch->driver->tft->textWidth("000%", 4);
    batteryTextArea.h = watch->driver->tft->fontHeight(4);
    batteryTextArea.x = 120 - (batteryTextArea.w / 2);
    batteryTextArea.y = timeTextArea.y - batteryTextArea.h - 8;
    wipeBatteryArea = batteryTextArea;

    // Check if the watch started up while charging
    charging = watch->driver->power->isChargeing();

    // Trigger changes
    wasCharging = !charging;
    refreshBatteryPercent = true;
    lastMinute--;
    lastDay--;
    batteryPercentage = 1.1f;
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
    if (!watch->display.IsEnabled())
    {
        return;
    }

    switch (e.type)
    {
    case EVENT_POWER_DISCONNECT:
        charging = false;
        wasCharging = true;
        break;
    case EVENT_POWER_CONNECT:
        charging = true;
        wasCharging = false;
        watch->driver->shake();
        break;
    case EVENT_POWER_CHARGE:
        break;
    case EVENT_POWER_BUTTON:
        refreshBatteryPercent = true;
        break;
    default:
        break;
    }

}

void Homestead::Render(Display& display)
{
    // TODO: consider using timer instead of RTC to update every minute, maybe more efficient?
    RTC_Date date = watch->driver->rtc->getDateTime();
    if (lastMinute != date.minute)
    {
        lastMinute = date.minute;

        // Convert date to text
        char text[6] = { '\0' };
        sprintf(text, "%02u:%02u", date.hour, date.minute);

        // Now overwrite previous text
        display.GetTFT()->setTextSize(3);
        display.GetTFT()->setTextColor(TFT_WHITE);
        timeTextArea.DrawFilled(display, TFT_BLACK);
        display.GetTFT()->drawString(text, timeTextArea.x, timeTextArea.y, 4);
    }

    if (lastDay != date.day)
    {
        lastDay = date.day;

        // TODO: remove this hacky time fix... should sync to WiFi.
        uint8_t day = date.day - 1;

        // Now overwrite previous text
        display.GetTFT()->setTextSize(1);
        display.GetTFT()->setTextColor(TFT_WHITE);

        // Convert date to text
        char text[13] = { '\0' };
        sprintf(text, "%.3s %d%s %.3s", GetWeekdayName(watch->driver->rtc->getDayOfWeek(day, date.month, date.year)), day, GetNumericSuffix(day), GetMonthName(date.month));
        //sprintf(text, watch->driver->rtc->formatDateTime(PCF_TIMEFORMAT_DD_MM_YYYY));

        wipeDateArea.DrawFilled(display, TFT_BLACK);
        // Width can change
        dateTextArea.w = display.GetTFT()->textWidth(text, 4);
        dateTextArea.x = 120 - (dateTextArea.w / 2);

        display.GetTFT()->drawString(text, dateTextArea.x, dateTextArea.y, 4);
    }

    // Update battery percentage every BATTERY_REFRESH_TIME seconds, or on forced refresh.
    if ((lastSecond != date.second && date.second % BATTERY_REFRESH_TIME == 0) || refreshBatteryPercent)
    {
        float currentBatteryPercentage = (float)watch->driver->power->getBattPercentage() / 100.0f;

        // No need to update unless battery percentage actually changes
        if (currentBatteryPercentage < batteryPercentage || (currentBatteryPercentage > batteryPercentage && charging))
        {
            batteryPercentage = Clamp(currentBatteryPercentage, 0.0f, 1.0f);
            LogLine(0, "Battery percent = %f", currentBatteryPercentage);

            // Draw text first
            display.GetTFT()->setTextSize(1);
            display.GetTFT()->setTextColor(TFT_WHITE);

            char text[5] = { '\0', '\0', '\0', '\0', '\0' };
            sprintf(text, "%d%%", (int)(batteryPercentage * 100.0f));

            wipeBatteryArea.DrawFilled(display, TFT_BLACK);
            // Width can change
            batteryTextArea.w = display.GetTFT()->textWidth(text, 4);
            batteryTextArea.x = 120 - (batteryTextArea.w / 2);

            display.GetTFT()->drawString(text, batteryTextArea.x, batteryTextArea.y, 4);
        }

    }

    if (wasCharging != charging)
    {
        if (charging)
        {
            // Show charging symbol
            // TODO
        }

        wasCharging = charging;

        // Now draw the fancy overlay
        for (uint8_t i = 0; i < 2; i++)
        {
            uint8_t dimensions = 240 - (i * 2);
            display.GetTFT()->drawRoundRect(i, i, dimensions, dimensions, 32 - i * 2, charging ? TFT_GREEN : TFT_BLUE);
        }
    }

    lastSecond = date.second;
}
