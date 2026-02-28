// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

#ifndef HIGHSCORE_H
#define HIGHSCORE_H
#include <EEPROM.h>

byte getAddressForGame(int gameNum) {
  return (gameNum * sizeof(byte)) + 1; // Start from address 1 to avoid overwriting the clear bit
}

void saveHighScore(int score, int gameNum) {
  byte highScore;
  int boundedScore = (score < 0) ? 0 : ((score > 255) ? 255 : score);
  byte scoreToSave = (byte)boundedScore;
  EEPROM.get(getAddressForGame(gameNum), highScore);
  if (scoreToSave > highScore) {
    EEPROM.put(getAddressForGame(gameNum), scoreToSave);
  }
}

byte getHighScore(int gameNum) {
  byte highScore;
  EEPROM.get(getAddressForGame(gameNum), highScore);
  return highScore;
}
#endif
