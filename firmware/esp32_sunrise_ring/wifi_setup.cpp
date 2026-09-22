#include "wifi_setup.h"
#include "config.h"

#include <Arduino.h>
#include <WiFiManager.h>
#include <string.h>

static WiFiManager wm;

static void onAPModeCallback(WiFiManager *mgr) {
  Serial.printf("[wifi] no known network found - portal '%s' started, connect to it and browse to %s\n",
                 mgr->getConfigPortalSSID().c_str(), WiFi.softAPIP().toString().c_str());
}

static void onSaveConfigCallback() {
  Serial.println("[wifi] new Wi-Fi credentials received, saving and reconnecting");
}

void wifiBegin() {
  wm.setConfigPortalBlocking(false);
  wm.setConfigPortalTimeout(0); // 0 = stay open indefinitely until configured
  wm.setAPCallback(onAPModeCallback);
  wm.setSaveConfigCallback(onSaveConfigCallback);

  if (strlen(AP_PASSWORD) >= 8) {
    wm.autoConnect(AP_NAME, AP_PASSWORD);
  } else {
    wm.autoConnect(AP_NAME); // open network
  }
}

void wifiLoop() {
  wm.process();
}

bool wifiIsConnected() {
  return WiFi.status() == WL_CONNECTED;
}
