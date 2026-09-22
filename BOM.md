# Bill of Materials

| Item | Qty | Notes | Reference |
|---|---|---|---|
| ESP32-C3 Super Mini | 1 | RISC-V, Wi-Fi, USB-C, native USB CDC | SKU `UG152251` |
| WS2812B RGB LED ring, 12 LEDs, 50mm | 1 | Single-wire addressable, 4 pads: 5V / GND / D0 (DI) / D1 (DO) | SKU `6QMK259415` |
| USB-C cable | 1 | For flashing and/or power | any |
| 5V USB power adapter | 1 | e.g. a phone charger; ~500mA is comfortably enough at the default low-brightness settings | any |
| Wiring, 3x | -- | 5V, GND, data -- jumper/Dupont wires or soldered wire | any |
| 3D-printed enclosure/mount | 1 set | see [`hardware/CrystalRita.3mf`](hardware/CrystalRita.3mf) | printed in-house |
| Filament | -- | your choice of color/material | -- |

## Notes

- The two SKU codes above are product/listing reference codes, not personal order numbers -- included so the exact same parts can be sourced again.
- No level shifter is included by default. The ESP32-C3's 3.3V GPIO logic driving the ring's 5V-referenced data line usually works fine over a short wire run; add a level shifter, or a 300-500 ohm series resistor on the data line, only if you see flicker or wrong colors.
- Total cost is low-end hobby-project territory (a few dollars/euros for the board and ring); any generic USB-C cable and 5V/500mA+ adapter works.
