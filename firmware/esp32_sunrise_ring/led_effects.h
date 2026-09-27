#pragma once

// Sets up the NeoPixel ring. Call once from setup().
void ledInit();

// Flashes all pixels solid red 3x rapidly (~0.7s total, blocking). Call once at the very
// start of setup(), unconditionally, regardless of Wi-Fi state -- a simple "I'm alive" signal.
void ledBootFlash();

// Slow breathing/pulse animation shown while offline / waiting for Wi-Fi setup.
void ledOfflineBreathe(unsigned long nowMs);

// Drives the day/night + sunrise/sunset-transition effect. Pass -1 for any argument
// that isn't known yet (e.g. time not synced); the effect falls back safely to "night".
void ledUpdate(unsigned long nowMs, int nowMinutes, int sunriseMinutes, int sunsetMinutes);

// Name of whichever steady-phase preset (Off/Dim glow/Breathe/Rainbow/Comet) is currently
// selected, or "-" during a sunrise/sunset transition. For remote diagnostics (e.g. the
// web settings page) when Serial isn't reachable.
const char *ledCurrentPresetName();

// Milliseconds since the last preset switch in the current steady phase (0 if not
// currently in one). Lets remote diagnostics (e.g. the web page) directly observe
// whether the rotation timer is actually advancing and using the configured interval.
unsigned long ledMillisSinceLastSwitch();
