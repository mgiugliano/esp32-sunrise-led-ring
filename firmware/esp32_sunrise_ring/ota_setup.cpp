#include "ota_setup.h"
#include "config.h"
#include "watchdog.h"

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <string.h>

void otaBegin() {
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  if (strlen(OTA_PASSWORD) > 0) {
    ArduinoOTA.setPassword(OTA_PASSWORD);
  }

  // A flash write during an update can take longer than one loop() pass,
  // so feed the watchdog explicitly from OTA's own callbacks too.
  ArduinoOTA.onStart([]() {
    Serial.println("[ota] update starting");
    watchdogFeed();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    static int lastPct = -1;
    int pct = total ? (int)((progress * 100UL) / total) : 0;
    if (pct != lastPct) {
      lastPct = pct;
      Serial.printf("[ota] progress: %d%%\n", pct);
    }
    watchdogFeed();
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("[ota] update complete, rebooting");
    watchdogFeed();
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("[ota] error [%u]\n", (unsigned)error);
    watchdogFeed();
  });

  ArduinoOTA.begin();
  Serial.printf("[ota] ready, hostname=%s\n", OTA_HOSTNAME);
}

void otaLoop() {
  ArduinoOTA.handle();
}
