// TWSUGamerPlus — seven-game sketch for the TWSU DIY Gamer Kit.
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

#include "Gamer.h"
#include <avr/pgmspace.h>
#include "src/assets/progmem_assets.h"
#include "src/persistence/highscore.h"

// ── Hardware instance ─────────────────────────────────────────────────────────
Gamer gamer;

// ── Shared globals ────────────────────────────────────────────────────────────
bool soundEnabled = false;
bool lastCapTouchState = false;
bool tetrisChirpPending = false;

unsigned long ledFlashStartTime = 0;
bool ledFlashing = false;

// gamer.playTone() note values: frequency ≈ 1,000,000 / (OCR2A + 1) Hz
#define NOTE_B7  252
#define NOTE_C8  238
#define NOTE_D8  212
#define NOTE_E8  189
#define NOTE_G8  158
#define NOTE_A8  140
#define NOTE_B8  125

#define WIN_NOTE_DURATION  100
#define LOSS_NOTE_DURATION 180

#define SIMON_MAX_SEQUENCE 30
#define SIMON_NUM_DIRECTIONS 4

// Feature flag: set to 1 to enable Serial debug output at startup.
#define GAMER_DEBUG 0

// Shared variables used by multiple games
int currentX = 0;
int currentY = 0;
int score    = 0;

// High score start feature
bool startFromHighScore = false;
byte startingHighScore = 0;

// ── Helper functions ──────────────────────────────────────────────────────────
// Common game loop preamble: check sound toggle, update LED flash, optionally stop tone
inline void updateGameInput(bool stopTone = true) {
  checkSoundToggle();
  updateLEDFlash();
  if (soundEnabled && stopTone) gamer.stopTone();
}

void startLEDFlash() {
  gamer.setLED(true);
  ledFlashStartTime = millis();
  ledFlashing = true;
}

inline void updateLEDFlash() {
  if (ledFlashing && millis() - ledFlashStartTime >= 175UL) {
    gamer.setLED(false);
    ledFlashing = false;
  }
}

// Detects a rising edge on the cap sense pad and toggles sound on/off
inline void checkSoundToggle() {
  bool cap = gamer.capTouch();
  if (cap && !lastCapTouchState) {
    soundEnabled = !soundEnabled;
    if (!soundEnabled) gamer.stopTone();
  }
  lastCapTouchState = cap;
}

// Plays a short ascending tune on win/success events
void playWinTune() {
  if (!soundEnabled) return;
  static const byte notes[] PROGMEM = {NOTE_C8, NOTE_E8, NOTE_G8, NOTE_B8};
  for (byte i = 0; i < 4; i++) { 
    gamer.playTone(pgm_read_byte(&notes[i])); 
    delay(WIN_NOTE_DURATION); 
  }
  gamer.stopTone();
}

// Plays a short descending tune on loss/fail events
void playLossTune() {
  if (!soundEnabled) return;
  static const byte notes[] PROGMEM = {NOTE_B8, NOTE_G8, NOTE_E8, NOTE_B7};
  for (byte i = 0; i < 4; i++) { 
    gamer.playTone(pgm_read_byte(&notes[i])); 
    delay(LOSS_NOTE_DURATION); 
  }
  gamer.stopTone();
}

// Score display using PROGMEM number bitmaps (3-pixel wide; tens shifted 5, units as-is)
void showScore(byte dig1, byte dig2) {
  byte result[8];
  for (byte p = 0; p < 8; p++)
    result[p] = (pgm_read_byte(&numbers_pgm[dig1][p]) << 5) | pgm_read_byte(&numbers_pgm[dig2][p]);
  gamer.printImage(result);
}

// ── Game implementations ──────────────────────────────────────────────────────
#include "src/games/snake.h"
#include "src/games/breakout.h"
#include "src/games/simon.h"
#include "src/games/flappy.h"
#include "src/games/tetris.h"
#include "src/games/alien.h"
#include "src/games/conway.h"
#include "src/games/dino.h"
#include "src/games/brightness.h"

// ── Launcher ──────────────────────────────────────────────────────────────────
#include "src/launcher/launcher.h"

// ── Startup self-test ─────────────────────────────────────────────────────────
static void startupCheck() {
#if GAMER_DEBUG
  Serial.begin(9600);
  byte b = progmemSelfCheck();
  Serial.print(F("[BOOT] PROGMEM startup_pgm[0][0]=0x"));
  Serial.print(b, HEX);
  Serial.println(b == 0xFF ? F(" OK") : F(" WARN: unexpected value"));
  Serial.print(F("[BOOT] High score: "));
  for (byte i = 0; i < LAUNCHER_MAX_GAMES; i++) {
    Serial.print(_games[i].name);
    Serial.print(": ");
    Serial.print(getHighScore(i));
    Serial.print("  ");
  }
#endif
}

// ── Arduino entry points ──────────────────────────────────────────────────────
void setup() {
  gamer.begin();
  const int eepromLen = (int)EEPROM.length();
  if(EEPROM.read(0) != 1) {
    for (int i = 1; i < eepromLen; i++) {
      EEPROM.write(i, 0);
    }
    EEPROM.write(0, 1); // Mark as initialized
  }
  startupCheck();
  launcherSetup();
}

void loop() {
  launcherLoop();
}
