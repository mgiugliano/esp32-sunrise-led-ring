#pragma once

// Tries the last-saved Wi-Fi credentials; if that fails, opens an AP + captive portal
// (non-blocking) named AP_NAME so a phone/laptop can configure new credentials, which
// are then persisted by the library. Call once from setup().
void wifiBegin();

// Services the captive portal's web/DNS server. Call on every loop() iteration while
// not yet connected (i.e. while the portal may be active).
void wifiLoop();

bool wifiIsConnected();
