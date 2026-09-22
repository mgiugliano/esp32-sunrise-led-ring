#pragma once

// Starts ArduinoOTA. Call once, after Wi-Fi is connected as a station.
void otaBegin();

// Services pending OTA requests. Call on every loop() iteration once otaBegin() has run.
void otaLoop();
