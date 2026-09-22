# Install & Build Guide

## Hardware needed

See [BOM.md](BOM.md) for the full parts list.

## Wiring

Ring pads: `5V`, `GND`, `D0` (DI, data in), `D1` (DO, data out -- leave unconnected unless daisy-chaining a second ring).

| Ring pad | ESP32-C3 Super Mini pin |
|---|---|
| 5V | 5V |
| GND | GND |
| D0 (DI) | GPIO4 |
| D1 (DO) | not connected |

Pin silkscreen order can vary slightly between board batches/clones -- confirm against your own board's printed labels before wiring. GPIO4 is a safe default; avoid GPIO2/8/9 (strapping pins) and GPIO18/19 (native USB) on the ESP32-C3 Super Mini.

The ESP32-C3's GPIO logic is 3.3V while the ring's data line runs at 5V logic. A short wire (a few cm, as on this compact board) usually works fine without a level shifter. If you see flickering or wrong colors once wired up, add a level shifter or a 300-500 ohm series resistor on the data line.

## Arduino IDE setup

1. Install the `esp32` board package (Espressif Systems), core 3.x, via Boards Manager.
2. Board: "ESP32C3 Dev Module".
3. Tools menu: **USB CDC On Boot = Enabled** -- required for Serial output over the USB-C port. Without this, nothing will ever appear in the Serial Monitor, regardless of what the sketch prints.
4. Tools menu: Partition Scheme = one with two OTA app slots (e.g. "Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)").
5. Library Manager: install `WiFiManager` (tzapu), `Adafruit NeoPixel`, and `Sunset` (buelowp).

## Configure before flashing

Edit `firmware/esp32_sunrise_ring/config.h`:

- `LATITUDE` / `LONGITUDE` / `TZ_INFO` -- set to your install location (defaults to a Rome, Italy placeholder).
- `LED_PIN` -- defaults to GPIO4; change if you wired differently.
- Everything else (AP name, OTA hostname, default brightness, default effect masks) is optional to change.

## First flash

1. Connect via USB-C, select the correct serial port, Upload.
2. Open the Serial Monitor at 115200 baud -- you should see 3 quick red flashes on the ring immediately at boot, followed by diagnostic log lines.
3. If nothing shows in Serial: re-check "USB CDC On Boot = Enabled" (step 3 above), then re-select the port (it can re-enumerate after upload) and reopen the monitor.

## First-time Wi-Fi setup

1. On first boot (no saved credentials), the ring shows a slow amber breathing animation and the device opens a Wi-Fi access point named `LEDring-Setup` (open network, no password).
2. Connect to it from a phone or laptop; a captive portal page should open automatically (or browse to `192.168.4.1`).
3. Choose "Configure WiFi", select your network, enter its password, save.
4. The device reconnects, syncs time over NTP, and starts normal operation. Credentials are saved to flash and reused on every future boot -- if that saved network is ever out of range (e.g. you move the device), it automatically falls back to the portal again on the next power-up.

## Using the web settings page

Once running, browse to `http://esp32-ledring.local/` (or the IP address printed to Serial) from any device on the same network. From there you can:

- See status (current time, sunrise/sunset, uptime, free heap)
- Choose which effects are enabled for day vs. night (Off, Dim glow, Breathe, Rainbow, Comet)
- Set the rotation interval (seconds between pattern switches) and the Comet movement speed
- Trigger a 3x red test flash, to confirm the page is actually reaching the device

Settings are saved to the device's flash (NVS) and survive power cycles.

## Over-the-air (OTA) updates

Once connected, the device advertises itself via mDNS as `esp32-ledring` and should appear under Arduino IDE's Tools -> Port -> Network Ports. Select it and Upload as normal -- no USB cable needed. A password prompt appears regardless of whether one is set in `config.h`; if `OTA_PASSWORD` is left empty (the default), any value you type is accepted since the device doesn't check it.

## Resetting Wi-Fi credentials / all settings

Both Wi-Fi credentials and the effect settings live in the ESP32's flash (NVS), and both survive a normal re-flash. To fully reset either: Arduino IDE -> Tools -> "Erase All Flash Before Sketch Upload" -> Enabled, do one Upload, then set it back to Disabled (otherwise every future flash wipes them again).

## Troubleshooting

- **No Serial output at all**: see "USB CDC On Boot" above -- by far the most common cause on this board, since it has no separate USB-serial chip.
- **OTA device not showing in Network Ports**: a known, long-standing Arduino IDE 2.x bug, not a sketch issue. Try: `ping esp32-ledring.local` from a terminal (confirms mDNS is working, independent of the IDE); fully restart the IDE; wait ~30s after boot before checking; make sure your router isn't isolating Wi-Fi clients from each other (common on guest networks/mesh systems).
- **LEDs don't light up**: use `tools/led_pin_finder/` to sweep candidate GPIOs and visually confirm which one is actually wired to the ring; check 5V/GND continuity with a multimeter with the board powered off.
- **Ring stuck on dim, static blue**: means NTP time hasn't synced yet (no internet reachable on the network). It resumes normal day/night effects automatically once time syncs -- no action needed once internet access returns.
