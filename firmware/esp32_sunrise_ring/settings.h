#pragma once
#include <stdint.h>

struct RingSettings {
  uint8_t nightEffectsMask;        // bit i = preset i enabled at night (see config.h for bit meanings)
  uint8_t dayEffectsMask;          // bit i = preset i enabled during the day
  uint16_t cycleIntervalSeconds;   // seconds between preset switches within a steady phase
  uint16_t movementPeriodSeconds;  // seconds per full lap around the ring, for movement presets (Comet)
};

// Loads settings from flash (NVS), falling back to config.h defaults the first time. Call once in setup().
void settingsInit();

// Mutable reference to the current in-memory settings.
RingSettings &settingsGet();

// Persists the current in-memory settings to flash.
void settingsSave();
