#include "time_sun.h"
#include "config.h"

#include <Arduino.h>
#include <time.h>
#include <math.h>
#include <sunset.h>

static SunSet sun;
static unsigned long syncStartMs = 0;
static bool timeIsValid = false;
static int lastComputedYday = -1;
static int sunriseMin = -1;
static int sunsetMin = -1;

void timeSyncBegin() {
  Serial.println("[time] starting NTP sync...");
  configTzTime(TZ_INFO, NTP_SERVER_1, NTP_SERVER_2, NTP_SERVER_3);
  syncStartMs = millis();
  timeIsValid = false;
  lastComputedYday = -1; // force a sun-times recompute once time becomes valid
}

bool timeSyncCheck() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 10)) {
    if (!timeIsValid) {
      char buf[32];
      strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
      Serial.printf("[time] synced: %s local\n", buf);
    }
    timeIsValid = true;
  }
  return timeIsValid;
}

bool timeSyncTimedOut() {
  return (millis() - syncStartMs) > TIME_SYNC_TIMEOUT_MS;
}

static void computeSunTimes(const struct tm &timeinfo) {
  double tz = UTC_OFFSET_HOURS + (timeinfo.tm_isdst > 0 ? 1.0 : 0.0);
  sun.setPosition(LATITUDE, LONGITUDE, tz);
  sun.setCurrentDate(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);

  double sr = sun.calcSunrise();
  double ss = sun.calcSunset();
  sunriseMin = isnan(sr) ? -1 : (int)sr;
  sunsetMin = isnan(ss) ? -1 : (int)ss;
  lastComputedYday = timeinfo.tm_yday;

  Serial.printf("[sun] %04d-%02d-%02d sunrise=%02d:%02d sunset=%02d:%02d\n",
                 timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                 sunriseMin < 0 ? 0 : sunriseMin / 60, sunriseMin < 0 ? 0 : sunriseMin % 60,
                 sunsetMin < 0 ? 0 : sunsetMin / 60, sunsetMin < 0 ? 0 : sunsetMin % 60);
}

void sunTimesUpdateIfNeeded() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 10)) {
    return;
  }
  if (timeinfo.tm_yday != lastComputedYday) {
    computeSunTimes(timeinfo);
  }
}

int sunriseMinutesToday() { return sunriseMin; }
int sunsetMinutesToday() { return sunsetMin; }

int currentMinutesOfDay() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 10)) {
    return -1;
  }
  return timeinfo.tm_hour * 60 + timeinfo.tm_min;
}
