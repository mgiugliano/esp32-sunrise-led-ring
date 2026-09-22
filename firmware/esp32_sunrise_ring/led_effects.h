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
