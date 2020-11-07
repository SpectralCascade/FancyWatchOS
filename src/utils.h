#ifndef UTILS_H
#define UTILS_H

#include "config.h"

//#define LOG_SERIAL 1
//#define LOG_OVERLAY 1


#if LOG_OVERLAY
#define Log(...) TTGOClass::getWatch()->tft->printf(__VA_ARGS__); TTGOClass::getWatch()->tft->printf("\n")
#define LogLine(LINE, ...) TTGOClass::getWatch()->tft->setCursor(0, LINE * TTGOClass::getWatch()->tft->fontHeight()); TTGOClass::getWatch()->tft->printf(__VA_ARGS__);
#elif LOG_SERIAL
#define Log(...) Serial.printf(__VA_ARGS__); Serial.printf("\n")
#define LogLine(LINE, ...) Log(__VA_ARGS__)
#else
#define Log(...)
#define LogLine(...)
#endif

#define LogInfo(...) TTGOClass::getWatch()->tft->setTextColor(TFT_GREEN); Log(__VA_ARGS__)
#define LogWarn(...) TTGOClass::getWatch()->tft->setTextColor(TFT_ORANGE); Log(__VA_ARGS__)
#define LogError(...) TTGOClass::getWatch()->tft->setTextColor(TFT_RED); Log(__VA_ARGS__)
#define LogMark(fn, ln) Log("\nReached mark at line %d in file %s.\n", ln, fn)

/// Clamps between a range
float Clamp(float n, float min = 0, float max = 1);
int Clamp(int n, int min, int max);

/// Wraps an integer number within a given range. Both min and max are INCLUSIVE.
int Wrap(int n, int change, int min, int max);

/// Maps a value and it's range to a different range.
/// value = value to be mapped, min and max = original range, min_new and max_new = the new range to be mapped to.
float MapRange(float value, float min, float max, float min_new, float max_new);

#endif // UTILS_H
