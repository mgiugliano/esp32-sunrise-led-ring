#include "led_effects.h"
#include "config.h"
#include "settings.h"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

static Adafruit_NeoPixel ring(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Minutes on either side of sunrise/sunset treated as a gradient transition window.
static const int TRANSITION_WINDOW_MIN = 30;

static void fillAll(uint32_t color) {
  for (int i = 0; i < LED_COUNT; i++) {
    ring.setPixelColor(i, color);
  }
  ring.show();
}

void ledInit() {
  ring.begin();
  ring.setBrightness(LED_BRIGHTNESS);
  ring.clear();
  ring.show();
  Serial.printf("[led] ring initialized: %d pixels on GPIO%d, brightness=%d\n",
                 LED_COUNT, LED_PIN, LED_BRIGHTNESS);
}

void ledBootFlash() {
  for (int i = 0; i < 3; i++) {
    fillAll(ring.Color(255, 0, 0));
    delay(120);
    ring.clear();
    ring.show();
    delay(120);
  }
}

void ledOfflineBreathe(unsigned long nowMs) {
  const unsigned long cycleMs = 4000;
  float phase = (nowMs % cycleMs) / (float)cycleMs;
  float level = (sinf(phase * 2.0f * PI) + 1.0f) / 2.0f; // 0..1
  uint8_t v = (uint8_t)(level * 255);
  fillAll(ring.Color(v, v / 3, 0)); // warm amber breathing
}

// --- Steady-phase preset rotation (day/night only -- sunrise/sunset transitions below
// are dedicated gradients, untouched by this). Bit positions match config.h's defaults. ---

enum Preset {
  PRESET_OFF = 0,
  PRESET_DIM_GLOW = 1,
  PRESET_BREATHE = 2,
  PRESET_RAINBOW = 3,
  PRESET_COMET = 4,
  NUM_PRESETS = 5
};

// Keep in sync with web_ui.cpp's own copy (kept separate to avoid cross-module coupling).
static const char *PRESET_NAMES[NUM_PRESETS] = {"Off", "Dim glow", "Breathe", "Rainbow", "Comet"};

static int currentPresetIndex = PRESET_OFF;
static int lastPhaseKind = -1; // -1 = none/transition, 0 = night, 1 = day
static unsigned long lastSwitchMs = 0;

static void renderPreset(int preset, unsigned long nowMs, bool isNight) {
  switch (preset) {
    case PRESET_OFF:
      ring.clear();
      ring.show();
      break;
    case PRESET_DIM_GLOW:
      fillAll(isNight ? ring.Color(20, 8, 0) : ring.Color(12, 12, 16));
      break;
    case PRESET_BREATHE: {
      const unsigned long cycleMs = 6000;
      float phase = (nowMs % cycleMs) / (float)cycleMs;
      float level = (sinf(phase * 2.0f * PI) + 1.0f) / 2.0f;
      uint8_t v = (uint8_t)(level * 255);
      fillAll(isNight ? ring.Color(0, 0, v) : ring.Color(v, v, v));
      break;
    }
    case PRESET_RAINBOW: {
      uint16_t hue = (uint16_t)((nowMs / 20) % 65536);
      fillAll(ring.gamma32(ring.ColorHSV(hue, 255, 200)));
      break;
    }
    case PRESET_COMET: {
      unsigned long periodMs = (unsigned long)settingsGet().movementPeriodSeconds * 1000UL;
      if (periodMs == 0) {
        periodMs = 1000UL;
      }
      float posFraction = (float)(nowMs % periodMs) / (float)periodMs; // 0..1 around the ring
      int headIdx = (int)(posFraction * LED_COUNT);

      ring.clear();
      const int tailLen = 4;
      for (int t = 0; t < tailLen; t++) {
        int idx = ((headIdx - t) % LED_COUNT + LED_COUNT) % LED_COUNT;
        uint8_t val = 255 - (uint8_t)(t * (255 / tailLen));
        ring.setPixelColor(idx, ring.gamma32(ring.ColorHSV(43000, 180, val)));
      }
      ring.show();
      break;
    }
  }
}

// Returns the next enabled preset after fromIndex (wrapping), or PRESET_OFF if none enabled.
static int nextEnabledPreset(uint8_t mask, int fromIndex) {
  for (int step = 1; step <= NUM_PRESETS; step++) {
    int idx = (fromIndex + step) % NUM_PRESETS;
    if (mask & (1 << idx)) {
      return idx;
    }
  }
  return PRESET_OFF;
}

static void updateStablePhase(unsigned long nowMs, bool isNight) {
  RingSettings &s = settingsGet();
  uint8_t mask = isNight ? s.nightEffectsMask : s.dayEffectsMask;
  unsigned long intervalMs = (unsigned long)s.cycleIntervalSeconds * 1000UL;
  int phaseKind = isNight ? 0 : 1;

  if (phaseKind != lastPhaseKind) {
    lastPhaseKind = phaseKind;
    currentPresetIndex = nextEnabledPreset(mask, -1);
    lastSwitchMs = nowMs;
    Serial.printf("[led] phase=%s preset=%s\n", isNight ? "night" : "day", PRESET_NAMES[currentPresetIndex]);
  } else if (intervalMs > 0 && nowMs - lastSwitchMs >= intervalMs) {
    currentPresetIndex = nextEnabledPreset(mask, currentPresetIndex);
    lastSwitchMs = nowMs;
    Serial.printf("[led] phase=%s preset=%s\n", isNight ? "night" : "day", PRESET_NAMES[currentPresetIndex]);
  }

  renderPreset(currentPresetIndex, nowMs, isNight);
}

static void effectSunriseTransition(float progress) {
  uint8_t r = (uint8_t)(progress * 255);
  uint8_t g = (uint8_t)(progress * 120);
  uint8_t b = (uint8_t)((1.0f - progress) * 60);
  fillAll(ring.Color(r, g, b));
}

static void effectSunsetTransition(float progress) {
  uint8_t r = (uint8_t)((1.0f - progress) * 255);
  uint8_t g = (uint8_t)((1.0f - progress) * 100);
  uint8_t b = (uint8_t)(progress * 60);
  fillAll(ring.Color(r, g, b));
}

void ledUpdate(unsigned long nowMs, int nowMinutes, int sunriseMinutes, int sunsetMinutes) {
  if (nowMinutes < 0 || sunriseMinutes < 0 || sunsetMinutes < 0) {
    lastPhaseKind = -1; // not a stable phase; re-enter cleanly once data is valid
    fillAll(ring.Color(0, 0, 10)); // safe dim-blue fallback until time/sun data is valid
    return;
  }

  int sunriseStart = sunriseMinutes - TRANSITION_WINDOW_MIN;
  int sunriseEnd   = sunriseMinutes + TRANSITION_WINDOW_MIN;
  int sunsetStart  = sunsetMinutes - TRANSITION_WINDOW_MIN;
  int sunsetEnd    = sunsetMinutes + TRANSITION_WINDOW_MIN;

  if (nowMinutes >= sunriseStart && nowMinutes < sunriseEnd) {
    lastPhaseKind = -1;
    float progress = (float)(nowMinutes - sunriseStart) / (2 * TRANSITION_WINDOW_MIN);
    effectSunriseTransition(progress);
  } else if (nowMinutes >= sunsetStart && nowMinutes < sunsetEnd) {
    lastPhaseKind = -1;
    float progress = (float)(nowMinutes - sunsetStart) / (2 * TRANSITION_WINDOW_MIN);
    effectSunsetTransition(progress);
  } else if (nowMinutes >= sunriseEnd && nowMinutes < sunsetStart) {
    updateStablePhase(nowMs, false); // day
  } else {
    updateStablePhase(nowMs, true); // night
  }
}
