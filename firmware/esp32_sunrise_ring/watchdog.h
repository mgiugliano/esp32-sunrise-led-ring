#pragma once

// Initializes the ESP32 hardware task watchdog and registers the loop() task with it.
// Call once, first thing in setup(), before anything that could hang.
void watchdogInit();

// Feeds (resets) the watchdog. Call unconditionally on every loop() iteration,
// and also from any callback that can block for a while (e.g. OTA progress).
void watchdogFeed();
