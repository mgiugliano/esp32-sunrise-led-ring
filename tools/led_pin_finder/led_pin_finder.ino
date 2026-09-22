// Diagnostic sketch: sweeps through candidate GPIOs on the ESP32-C3 Super Mini,
// driving all 12 WS2812B pixels as a solid red on each pin in turn.
// Watch the ring and the Serial Monitor (115200 baud) together: whichever pin
// number is printed when the ring flashes red is the pin actually wired to
// the ring's D0 (data-in) pad.
//
// Only needs the "Adafruit NeoPixel" library. Independent of the main project
// sketch -- flash this instead, temporarily, just to confirm the wiring/pin.

#include <Adafruit_NeoPixel.h>

#define LED_COUNT   12
#define BRIGHTNESS  120

// Candidate data pins to test, in silkscreen order. Skips 8/9 (strapping pins;
// 8 also drives the board's own onboard addressable LED, which would confuse
// the test) and 18/19/20/21 (native USB / UART).
const uint8_t candidatePins[] = {0, 1, 2, 3, 4, 5, 6, 7, 10};
const size_t numCandidates = sizeof(candidatePins) / sizeof(candidatePins[0]);

void testPin(uint8_t pin) {
  Serial.printf("Testing GPIO%d ...\n", pin);

  Adafruit_NeoPixel ring(LED_COUNT, pin, NEO_GRB + NEO_KHZ800);
  ring.begin();
  ring.setBrightness(BRIGHTNESS);
  ring.fill(ring.Color(255, 0, 0)); // solid red
  ring.show();
  delay(1500);
  ring.clear();
  ring.show();
  delay(300);
}

void setup() {
  Serial.begin(115200);
  unsigned long t0 = millis();
  while (!Serial && millis() - t0 < 2000) {
    delay(10);
  }
  Serial.println();
  Serial.println("=== LED ring pin finder ===");
  Serial.println("Watch the ring: it should flash solid RED for 1.5s on the correct pin.");
}

void loop() {
  for (size_t i = 0; i < numCandidates; i++) {
    testPin(candidatePins[i]);
  }
  Serial.println("--- sweep complete, restarting ---");
  delay(1000);
}
