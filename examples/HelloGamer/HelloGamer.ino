/*
 * HelloGamer — minimal Gamer library example
 *
 * Demonstrates the core Gamer API:
 *   - gamer.begin()           initialise hardware
 *   - gamer.display[x][y]     set individual pixels
 *   - gamer.updateDisplay()   push buffer to the 8×8 LED matrix
 *   - gamer.clear()           blank the display
 *   - gamer.isPressed(btn)    edge-triggered button read
 *   - gamer.playTone(note)    start a buzzer tone
 *   - gamer.stopTone()        stop the buzzer
 *   - gamer.setLED(true/false) control the onboard LED
 *
 * Usage:
 *   1. Place this sketch and Gamer.h/Gamer.cpp in the same directory,
 *      or install the library via the Arduino Library Manager.
 *   2. Select Board: Arduino Uno and the correct serial port.
 *   3. Upload.
 *
 * What you will see:
 *   - A 3×3 checkerboard scrolls diagonally across the display.
 *   - Press UP   to flash all pixels on.
 *   - Press DOWN to clear the display.
 *   - Press START to toggle a short beep.
 *
 * Hardware: TWSU DIY Gamer Kit (ATmega328P, 8×8 LED matrix)
 * SPDX-License-Identifier: MIT
 */

#include "Gamer.h"

Gamer gamer;

// Note value for gamer.playTone(): OCR2A register byte
// Frequency ≈ 1,000,000 / (note + 1) Hz
static const int NOTE_A4 = 227;  // ~440 Hz

void setup() {
  gamer.begin();
}

void loop() {
  static unsigned long lastMove = 0;
  static byte offset = 0;

  // Draw a scrolling checkerboard
  if (millis() - lastMove >= 150) {
    lastMove = millis();

    for (byte x = 0; x < 8; x++) {
      for (byte y = 0; y < 8; y++) {
        // Checkerboard pattern shifted by offset
        gamer.display[x][y] = ((x + y + offset) & 1);
      }
    }
    gamer.updateDisplay();
    offset++;
  }

  // UP: flash all pixels on for 300 ms
  if (gamer.isPressed(UP)) {
    gamer.allOn();
    delay(300);
  }

  // DOWN: clear display
  if (gamer.isPressed(DOWN)) {
    gamer.clear();
    delay(300);
  }

  // START: short beep and LED flash
  if (gamer.isPressed(START)) {
    gamer.setLED(true);
    gamer.playTone(NOTE_A4);
    delay(200);
    gamer.stopTone();
    gamer.setLED(false);
  }
}
