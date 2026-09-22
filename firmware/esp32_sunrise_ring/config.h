#pragma once

// ---- Wi-Fi captive portal ----
#define AP_NAME       "LEDring-Setup"
#define AP_PASSWORD   ""   // "" = open network; if set, must be >= 8 characters (SoftAP requirement)

// ---- Watchdog ----
#define WDT_TIMEOUT_MS   15000UL   // reboot if loop() doesn't feed the watchdog within this long

// ---- LED ring (AZ-Delivery WS2812B, 12 LEDs, 50 mm) ----
// Ring pads: 5V, GND, D0 (DI, data in) -> wire to LED_PIN, D1 (DO, data out) -> leave unconnected
// unless daisy-chaining a second ring. Avoid GPIO2/8/9 (strapping pins) and GPIO18/19 (native USB)
// on the ESP32-C3 Super Mini.
#define LED_PIN          4
#define LED_COUNT        12
#define LED_BRIGHTNESS   24    // 0-255; kept low for power saving -- effects are designed around this

// ---- Effect cycling (day/night steady-phase preset rotation) ----
// Preset bit positions, shared by night/day masks: 0=Off, 1=Dim glow, 2=Breathe, 3=Rainbow, 4=Comet.
#define DEFAULT_NIGHT_EFFECTS_MASK   0b11110  // Dim glow + Breathe + Rainbow + Comet (Off disabled)
#define DEFAULT_DAY_EFFECTS_MASK     0b00011  // Off + Dim glow
#define DEFAULT_CYCLE_INTERVAL_SEC   600      // seconds between preset switches (10 min); can be set much shorter
#define DEFAULT_MOVEMENT_PERIOD_SEC  8        // seconds per full lap around the ring for movement presets (Comet)

// ---- Location & timezone (EDIT to your install site) ----
#define LATITUDE          41.9028   // placeholder: Rome, Italy -- change to your location
#define LONGITUDE         12.4964
#define UTC_OFFSET_HOURS  1.0       // standard-time UTC offset, no DST (e.g. CET = UTC+1)
#define TZ_INFO           "CET-1CEST,M3.5.0,M10.5.0/3"  // POSIX TZ string; handles DST automatically

// ---- NTP ----
#define NTP_SERVER_1          "pool.ntp.org"
#define NTP_SERVER_2          "time.google.com"
#define NTP_SERVER_3          "time.cloudflare.com"
#define TIME_SYNC_TIMEOUT_MS  30000UL

// ---- OTA ----
#define OTA_HOSTNAME      "esp32-ledring"
#define OTA_PASSWORD      ""   // "" = no password; set one for real deployments
