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

