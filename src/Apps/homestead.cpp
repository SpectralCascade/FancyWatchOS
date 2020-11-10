#include "homestead.h"

void Homestead::OnStart(int argc, char* argv[])
{
    if (IsForeground())
    {
        // May as well do this here, though should really do on foreground.
        watch->driver->tft->fillScreen(TFT_BLACK);
    }

    // Battery text
    batteryText.SetFont(4);
    batteryText.SetSize(1);
    batteryText.SetDatum(MC_DATUM);

    // Time text
    timeText.SetFont(4);
    timeText.SetSize(3);
    timeText.SetDatum(MC_DATUM);

    // Date text
    dateText.SetFont(4);
    dateText.SetSize(1);
    dateText.SetDatum(MC_DATUM);

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

        timeText.SetText(text);
        timeText.Render(display);
    }

    if (lastDay != date.day)
    {
        lastDay = date.day;

        // TODO: remove this hacky time fix... should sync to WiFi.
        uint8_t day = date.day - 1;

        // Convert date to text
        char text[13] = { '\0' };
        sprintf(text, "%.3s %d%s %.3s", GetWeekdayName(watch->driver->rtc->getDayOfWeek(day, date.month, date.year)), day, GetNumericSuffix(day), GetMonthName(date.month));
        //sprintf(text, watch->driver->rtc->formatDateTime(PCF_TIMEFORMAT_DD_MM_YYYY));

        dateText.SetText(text);
        dateText.Render(display);
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

            char text[5] = { '\0', '\0', '\0', '\0', '\0' };
            sprintf(text, "%d%%", (int)(batteryPercentage * 100.0f));

            batteryText.SetText(text);
            batteryText.Render(display);
        }

        refreshBatteryPercent = false;
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
