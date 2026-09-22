#include "settings.h"
#include "config.h"

#include <Arduino.h>
#include <Preferences.h>

static Preferences prefs;
static RingSettings settings;

void settingsInit() {
  prefs.begin("ledring", false);
  settings.nightEffectsMask = prefs.getUChar("nightMask", DEFAULT_NIGHT_EFFECTS_MASK);
  settings.dayEffectsMask = prefs.getUChar("dayMask", DEFAULT_DAY_EFFECTS_MASK);
  settings.cycleIntervalSeconds = prefs.getUShort("intervalSec", DEFAULT_CYCLE_INTERVAL_SEC);
  settings.movementPeriodSeconds = prefs.getUShort("moveSec", DEFAULT_MOVEMENT_PERIOD_SEC);
  prefs.end();

  Serial.printf("[settings] loaded: nightMask=0x%02X dayMask=0x%02X intervalSec=%u moveSec=%u\n",
                settings.nightEffectsMask, settings.dayEffectsMask,
                settings.cycleIntervalSeconds, settings.movementPeriodSeconds);
}

RingSettings &settingsGet() {
  return settings;
}

void settingsSave() {
  prefs.begin("ledring", false);
  prefs.putUChar("nightMask", settings.nightEffectsMask);
  prefs.putUChar("dayMask", settings.dayEffectsMask);
  prefs.putUShort("intervalSec", settings.cycleIntervalSeconds);
  prefs.putUShort("moveSec", settings.movementPeriodSeconds);
  prefs.end();

  Serial.printf("[settings] saved: nightMask=0x%02X dayMask=0x%02X intervalSec=%u moveSec=%u\n",
                settings.nightEffectsMask, settings.dayEffectsMask,
                settings.cycleIntervalSeconds, settings.movementPeriodSeconds);
}
