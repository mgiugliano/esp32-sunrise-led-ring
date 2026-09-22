#include "config.h"
#include "watchdog.h"
#include "wifi_setup.h"
#include "ota_setup.h"
#include "time_sun.h"
#include "led_effects.h"
#include "settings.h"
#include "web_ui.h"

#include <WiFi.h>

enum State {
  STATE_PORTAL_OFFLINE,
  STATE_TIME_SYNC,
  STATE_RUNNING
};

static State state; // zero-initialized -> STATE_PORTAL_OFFLINE
static unsigned long lastHeartbeatMs = 0;

static const char *stateName(State s) {
  switch (s) {
    case STATE_PORTAL_OFFLINE: return "PORTAL_OFFLINE";
    case STATE_TIME_SYNC:      return "TIME_SYNC";
    case STATE_RUNNING:        return "RUNNING";
  }
  return "?";
}

static void setState(State s) {
  Serial.printf("[main] state: %s -> %s\n", stateName(state), stateName(s));
  state = s;
}

void setup() {
  Serial.begin(115200);
  // Give a native-USB CDC port a moment to enumerate so early prints aren't lost
  // if a monitor is already open; harmless (just times out) if nothing is listening.
  unsigned long serialWaitStart = millis();
  while (!Serial && millis() - serialWaitStart < 2000) {
    delay(10);
  }
  delay(200);

  Serial.println();
  Serial.println("=== ESP32-C3 LED ring booting ===");
  Serial.printf("[main] chip=%s freq=%uMHz freeHeap=%u bytes\n",
                 ESP.getChipModel(), ESP.getCpuFreqMHz(), ESP.getFreeHeap());

  watchdogInit(); // first: catches hangs in anything that follows too
  ledInit();
  settingsInit();
  ledBootFlash(); // unconditional "I'm alive" signal, regardless of Wi-Fi state

  Serial.println("[wifi] trying saved credentials...");
  wifiBegin();

  if (wifiIsConnected()) {
    WiFi.setSleep(true); // modem-sleep for lower average power
    Serial.printf("[wifi] connected to '%s', IP=%s, RSSI=%ddBm\n",
                   WiFi.SSID().c_str(), WiFi.localIP().toString().c_str(), WiFi.RSSI());
    timeSyncBegin();
    setState(STATE_TIME_SYNC);
  } else {
    setState(STATE_PORTAL_OFFLINE);
  }
}

void loop() {
  watchdogFeed();
  unsigned long nowMs = millis();

  switch (state) {

    case STATE_PORTAL_OFFLINE:
      wifiLoop();
      ledOfflineBreathe(nowMs);
      if (wifiIsConnected()) {
        WiFi.setSleep(true); // modem-sleep for lower average power
        Serial.printf("[wifi] connected to '%s', IP=%s, RSSI=%ddBm\n",
                       WiFi.SSID().c_str(), WiFi.localIP().toString().c_str(), WiFi.RSSI());
        timeSyncBegin();
        setState(STATE_TIME_SYNC);
      }
      break;

    case STATE_TIME_SYNC:
      ledOfflineBreathe(nowMs); // keep some feedback while waiting for NTP
      if (timeSyncCheck()) {
        sunTimesUpdateIfNeeded();
        otaBegin();
        setState(STATE_RUNNING);
      } else if (timeSyncTimedOut()) {
        Serial.println("[time] NTP sync timed out, continuing without it");
        otaBegin();
        setState(STATE_RUNNING);
      }
      break;

    case STATE_RUNNING: {
      static bool runningSetupDone = false;
      if (!runningSetupDone) {
        setCpuFrequencyMhz(80); // steady-state power saving; still plenty for Wi-Fi/OTA/web
        webUiBegin();
        Serial.println("[main] steady-state: CPU scaled to 80MHz, web settings server ready");
        runningSetupDone = true;
      }

      otaLoop();
      webUiLoop();
      sunTimesUpdateIfNeeded();
      int nowMin = currentMinutesOfDay();
      int sunriseMin = sunriseMinutesToday();
      int sunsetMin = sunsetMinutesToday();
      ledUpdate(nowMs, nowMin, sunriseMin, sunsetMin);

      if (nowMs - lastHeartbeatMs > 10000) {
        lastHeartbeatMs = nowMs;
        Serial.printf("[main] heartbeat: uptime=%lus heap=%u RSSI=%ddBm now=%02d:%02d sunrise=%02d:%02d sunset=%02d:%02d\n",
                       nowMs / 1000, ESP.getFreeHeap(), WiFi.RSSI(),
                       nowMin < 0 ? 0 : nowMin / 60, nowMin < 0 ? 0 : nowMin % 60,
                       sunriseMin < 0 ? 0 : sunriseMin / 60, sunriseMin < 0 ? 0 : sunriseMin % 60,
                       sunsetMin < 0 ? 0 : sunsetMin / 60, sunsetMin < 0 ? 0 : sunsetMin % 60);
      }
      break;
    }
  }
}
