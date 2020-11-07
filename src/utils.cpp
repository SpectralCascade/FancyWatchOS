#include "utils.h"

float Clamp(float n, float min, float max)
{
    if (n < min)
    {
        return min;
    }
    else if (n > max)
    {
        return max;
    }
    return n;
}

int Clamp(int n, int min, int max)
{
    if (n < min)
    {
        return min;
    }
    else if (n > max)
    {
        return max;
    }
    return n;
}

int Wrap(int n, int change, int min, int max)
{
    int wrapped = n + change;
    if (change > max - min)
    {
        wrapped = n + (change % ((max - min) + 1));
    }
    else if (change < min - max)
    {
        wrapped = n + (change % ((max - min) + 1));
    }
    if (wrapped > max)
    {
        wrapped = min + (wrapped - (max + 1));
    }
    else if (wrapped < min)
    {
        wrapped = max - ((min - 1) - wrapped);
    }
    return wrapped;
}

float MapRange(float value, float min, float max, float min_new, float max_new)
{
    float fraction = Clamp(value, min, max) / (max - min);
    return min_new + (fraction * (max_new - min_new));
}

const char* GetWeekdayName(uint8_t dayOfWeek)
{
    static const char names[7][10] = {
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday",
        "Sunday"
    };

    dayOfWeek = Clamp(dayOfWeek, 1, 7) - 1;

    return names[dayOfWeek];
}

const char* GetMonthName(uint8_t monthOfYear)
{
    static const char names[12][10] = {
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December"
    };

    monthOfYear = Clamp(monthOfYear, 1, 12) - 1;

    return names[monthOfYear];
}

const char* GetNumericSuffix(uint32_t number)
{
    uint32_t normalised = number % 100;
    if (normalised > 10 && normalised < 14)
    {
        return "th";
    }

    normalised = normalised % 10;
    if (normalised != 0)
    {
        if (normalised < 2)
        {
            return "st";
        }
        else if (normalised < 3)
        {
            return "nd";
        }
        else if (normalised < 4)
        {
            return "rd";
        }
    }

    return "th";
}
