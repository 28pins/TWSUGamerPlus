// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

#ifndef HIGHSCORE_H
#define HIGHSCORE_H
#include <EEPROM.h>

byte getAddressForGame(int gameNum) {
  return (gameNum * sizeof(byte)) + 1; // Start from address 1 to avoid overwriting the clear bit
}

void saveHighScore(int score, int gameNum) {
  int highScore;
  EEPROM.get(getAddressForGame(gameNum), highScore);
  if (score > highScore) {
    EEPROM.put(getAddressForGame(gameNum), score);
  }
}

byte getHighScore(int gameNum) {
  byte highScore;
  EEPROM.get(getAddressForGame(gameNum), highScore);
  return highScore;
}
#endif