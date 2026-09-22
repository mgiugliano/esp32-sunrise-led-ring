#pragma once

// Kicks off NTP sync via configTzTime(). Call once when entering the time-sync phase.
void timeSyncBegin();

// Non-blocking poll: returns true once the RTC has valid local time. Call repeatedly.
bool timeSyncCheck();

// True once we've been waiting for NTP longer than TIME_SYNC_TIMEOUT_MS.
bool timeSyncTimedOut();

// Recomputes today's sunrise/sunset (via the Sunset library) whenever the local
// calendar day changes. Safe to call every loop() iteration once time is valid.
void sunTimesUpdateIfNeeded();

// All of the following return minutes past local midnight, or -1 if not yet known.
int sunriseMinutesToday();
int sunsetMinutesToday();
int currentMinutesOfDay();
