#include "web_ui.h"
#include "settings.h"
#include "time_sun.h"
#include "led_effects.h"
#include "config.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

static WebServer server(80);

static const int NUM_PRESETS_UI = 5;
// Names for bit positions 0-4, matching config.h's DEFAULT_*_EFFECTS_MASK comment.
static const char *PRESET_NAMES[NUM_PRESETS_UI] = {"Off", "Dim glow", "Breathe", "Rainbow", "Comet"};

static String formatMinutes(int m) {
  if (m < 0) {
    return "--:--";
  }
  char buf[6];
  snprintf(buf, sizeof(buf), "%02d:%02d", m / 60, m % 60);
  return String(buf);
}

static void handleRoot() {
  RingSettings &s = settingsGet();

  String html;
  html += "<!DOCTYPE html><html><head><title>LED Ring Settings</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{font-family:sans-serif;max-width:480px;margin:20px auto;padding:0 12px}";
  html += "fieldset{margin-bottom:16px}label{display:block;margin:4px 0}</style></head><body>";
  html += "<h2>LED Ring Settings</h2>";

  html += "<p>Uptime: " + String(millis() / 1000) + "s &middot; Free heap: " + String(ESP.getFreeHeap()) + " bytes</p>";
  html += "<p>Now: " + formatMinutes(currentMinutesOfDay());
  html += " &middot; Sunrise: " + formatMinutes(sunriseMinutesToday());
  html += " &middot; Sunset: " + formatMinutes(sunsetMinutesToday()) + "</p>";

  html += "<form method='POST' action='/flash'><button type='submit'>Flash ring (test)</button></form><br>";

  html += "<form method='POST' action='/save'>";

  html += "<fieldset><legend>Night effects</legend>";
  for (int i = 0; i < NUM_PRESETS_UI; i++) {
    bool checked = s.nightEffectsMask & (1 << i);
    html += "<label><input type='checkbox' name='n" + String(i) + "'" + (checked ? " checked" : "") + "> " + PRESET_NAMES[i] + "</label>";
  }
  html += "</fieldset>";

  html += "<fieldset><legend>Day effects</legend>";
  for (int i = 0; i < NUM_PRESETS_UI; i++) {
    bool checked = s.dayEffectsMask & (1 << i);
    html += "<label><input type='checkbox' name='d" + String(i) + "'" + (checked ? " checked" : "") + "> " + PRESET_NAMES[i] + "</label>";
  }
  html += "</fieldset>";

  html += "<label>Rotation interval, i.e. time between pattern switches (seconds): ";
  html += "<input type='number' name='interval' min='5' max='21600' value='" + String(s.cycleIntervalSeconds) + "'></label><br><br>";

  html += "<label>Movement speed for Comet, i.e. seconds per lap around the ring (higher = slower): ";
  html += "<input type='number' name='movespeed' min='1' max='600' value='" + String(s.movementPeriodSeconds) + "'></label><br><br>";

  html += "<button type='submit'>Save</button>";
  html += "</form></body></html>";

  server.send(200, "text/html", html);
}

static void handleSave() {
  RingSettings &s = settingsGet();

  uint8_t nightMask = 0;
  uint8_t dayMask = 0;
  for (int i = 0; i < NUM_PRESETS_UI; i++) {
    if (server.hasArg("n" + String(i))) {
      nightMask |= (1 << i);
    }
    if (server.hasArg("d" + String(i))) {
      dayMask |= (1 << i);
    }
  }
  s.nightEffectsMask = nightMask;
  s.dayEffectsMask = dayMask;

  if (server.hasArg("interval")) {
    int v = server.arg("interval").toInt();
    if (v < 5) v = 5;
    if (v > 21600) v = 21600;
    s.cycleIntervalSeconds = (uint16_t)v;
  }

  if (server.hasArg("movespeed")) {
    int v = server.arg("movespeed").toInt();
    if (v < 1) v = 1;
    if (v > 600) v = 600;
    s.movementPeriodSeconds = (uint16_t)v;
  }

  settingsSave();

  String html = "<!DOCTYPE html><html><body><p>Settings saved.</p><p><a href='/'>Back</a></p></body></html>";
  server.send(200, "text/html", html);
}

static void handleFlash() {
  ledBootFlash(); // reuse the boot-time "I'm alive" signal as a quick web-UI connectivity test
  String html = "<!DOCTYPE html><html><body><p>Flashed.</p><p><a href='/'>Back</a></p></body></html>";
  server.send(200, "text/html", html);
}

void webUiBegin() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/flash", HTTP_POST, handleFlash);
  server.begin();
  Serial.printf("[web] settings page ready at http://%s/\n", WiFi.localIP().toString().c_str());
}

void webUiLoop() {
  server.handleClient();
}
