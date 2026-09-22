#pragma once

// Starts the settings HTTP server on port 80. Call once, after entering the RUNNING state
// (Wi-Fi connected; WiFiManager's own provisioning server has already torn itself down by then).
void webUiBegin();

// Services pending HTTP clients. Call on every loop() iteration once webUiBegin() has run.
void webUiLoop();
