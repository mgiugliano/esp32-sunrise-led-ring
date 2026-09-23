# ESP32 Sunrise/Sunset LED Ring

A WS2812B LED ring, driven by an ESP32-C3 Super Mini, that runs a light-effect schedule tied to sunrise and sunset at your location. Provisioned over a Wi-Fi captive portal, updated over the air, and configurable from a small built-in web page.

## Demo

[demo.mp4](demo.mp4)

## Features

- Wi-Fi captive portal for first-time setup (credentials stored on the device)
- Automatic fallback: if no known network is found at boot, the portal opens again
- Hardware watchdog: reboots automatically if the main loop hangs
- NTP time sync + sunrise/sunset calculation for your location
- Day/night effect rotation (5 presets: Off, Dim glow, Breathe, Rainbow, Comet), configurable per phase from a web page, including rotation speed and movement speed
- Over-the-air (OTA) firmware updates
- Low-power defaults: dim brightness, Wi-Fi modem-sleep, reduced CPU clock during normal operation

## Repository layout

```
firmware/esp32_sunrise_ring/   Arduino sketch -- open esp32_sunrise_ring.ino in the Arduino IDE
tools/led_pin_finder/          Standalone diagnostic sketch to find the ring's data pin
hardware/CrystalRita.3mf       3D-printable enclosure/mount
demo.mp4                       Short demo video
INSTALL.md                     Build, wiring, and flashing instructions
BOM.md                         Full parts list
```

See [INSTALL.md](INSTALL.md) to build one, and [BOM.md](BOM.md) for the parts list.

## Status

Personal/hobby project. Wiring, Wi-Fi provisioning, OTA, the watchdog, and the settings web page have been tested on real hardware. The day/night effects themselves are intentionally simple and are still being refined.
