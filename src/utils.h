#ifndef UTILS_H
#define UTILS_H

struct IntRect
{
    int x, y, w, h;
};

/// Clamps between a range
float Clamp(float n, float min = 0, float max = 1);
int Clamp(int n, int min, int max);

/// Wraps an integer number within a given range. Both min and max are INCLUSIVE.
int Wrap(int n, int change, int min, int max);

/// Maps a value and it's range to a different range.
/// value = value to be mapped, min and max = original range, min_new and max_new = the new range to be mapped to.
float MapRange(float value, float min, float max, float min_new, float max_new);

#endif // UTILS_H
