#include "watchdog.h"
#include "config.h"

#include <Arduino.h>
#include <esp_task_wdt.h>

void watchdogInit() {
  // The Arduino-ESP32 core 3.x may already have a default TWDT config installed;
  // deinit first so our own timeout/panic settings always take effect cleanly.
  esp_task_wdt_deinit();

  esp_task_wdt_config_t twdtConfig = {
    .timeout_ms = WDT_TIMEOUT_MS,
    .idle_core_mask = 0,      // only watch the task(s) we explicitly add, not idle tasks
    .trigger_panic = true,    // panic + reboot on timeout
  };

  esp_task_wdt_init(&twdtConfig);
  esp_task_wdt_add(NULL);     // watch the current (loop) task

  Serial.printf("[wdt] initialized, timeout=%lums\n", (unsigned long)WDT_TIMEOUT_MS);
}

void watchdogFeed() {
  esp_task_wdt_reset();
}
