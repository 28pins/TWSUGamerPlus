// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

#ifndef CONWAY_H
#define CONWAY_H

// ── Conway's Game of Life state ───────────────────────────────────────────────
#define CONWAY_STAGNATION_LIMIT 5

byte conwayCurr[8];
byte conwayNext[8];
byte conwayStuck;

void conwayRandomize() {
  for (byte i = 0; i < 8; i++) conwayCurr[i] = (byte)random(0, 256);
  conwayStuck = 0;
}

// Advance one generation; returns true if any cell changed.
bool conwayStep() {
  bool anyChange = false;
  for (byte y = 0; y < 8; y++) {
    conwayNext[y] = 0;
    for (byte x = 0; x < 8; x++) {
      byte alive = 0;
      for (int8_t dy = -1; dy <= 1; dy++)
        for (int8_t dx = -1; dx <= 1; dx++) {
          if (dx == 0 && dy == 0) continue;
          byte nx = (x + dx + 8) & 7;
          byte ny = (y + dy + 8) & 7;
          if ((conwayCurr[ny] >> nx) & 1) alive++;
        }
      bool curr = (conwayCurr[y] >> x) & 1;
      bool next = (alive == 3) || (curr && alive == 2);
      if (next) conwayNext[y] |= (1 << x);
      if (next != curr) anyChange = true;
    }
  }
  for (byte i = 0; i < 8; i++) conwayCurr[i] = conwayNext[i];
  return anyChange;
}

bool conwayStepSmall() {
  bool anyChange = false;
  for (byte y = 2; y < 6; y++) {
    conwayNext[y] = 0;
    for (byte x = 2; x < 6; x++) {
      byte alive = 0;
      for (int8_t dy = -1; dy <= 1; dy++)
        for (int8_t dx = -1; dx <= 1; dx++) {
          if (dx == 0 && dy == 0) continue;
          int8_t nx = (int8_t)x - 2 + dx;
          int8_t ny = (int8_t)y - 2 + dy;
          if (nx < 0) nx += 4;
          if (nx >= 4) nx -= 4;
          if (ny < 0) ny += 4;
          if (ny >= 4) ny -= 4;
          nx += 2;
          ny += 2;
          if ((conwayCurr[ny] >> nx) & 1) alive++;
        }
      bool curr = (conwayCurr[y] >> x) & 1;
      bool next = (alive == 3) || (curr && alive == 2);
      if (next) conwayNext[y] |= (1 << x);
      if (next != curr) anyChange = true;
    }
  }
  return anyChange;
}

void resetConway() { conwayRandomize(); }

void conwayLoop() {
  checkSoundToggle();
  updateLEDFlash();

  bool changed = conwayStep();
  if (!changed) {
    conwayStuck++;
    if (conwayStuck > CONWAY_STAGNATION_LIMIT) conwayRandomize();
  } else {
    conwayStuck = 0;
  }

  if (gamer.isPressed(RIGHT)) conwayRandomize();

  for (byte x = 0; x < 8; x++)
    for (byte y = 0; y < 8; y++)
      gamer.display[x][y] = (conwayCurr[y] >> x) & 1;
  gamer.updateDisplay();
  delay(180);
}

#endif // CONWAY_H
