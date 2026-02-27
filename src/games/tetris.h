// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

#ifndef TETRIS_H
#define TETRIS_H

// ── Tetris game state ─────────────────────────────────────────────────────────
// score, currentX, currentY declared in main INO
unsigned long moveInterval = 1200;
int level = 1;
int linesCleared = 0;
bool gameOverT = false;
long lastDownPressTime = 0;
bool isInDownPress = false;
const int gridWidth = 8;
const int gridHeight = 8;
byte linesSinceLastDrop = 0;
int grid[gridHeight][gridWidth] = {
  {0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}
};
int currentPiece[3][3] = {
  {0,0,0},
  {0,0,0},
  {0,0,0}
};

enum PieceType { I, O, T, S, Z, J, L };
PieceType currentPieceType;

bool canMove(int x, int y, int piece[3][3]);
inline bool canMove(int x, int y) { return canMove(x, y, currentPiece); }
bool canMove(int x, int y, int piece[3][3]) {
  for (byte i = 0; i < 3; i++) {
    for (byte j = 0; j < 3; j++) {
      if (piece[i][j] == 1) {
        int newX = x + j;
        int newY = y + i;
        // Boundary checks: allow newY down to -3 (off-screen top) but never < -3
        if (newX < 0 || newX >= gridWidth || newY < -3 || newY >= gridHeight) {
          return false;
        }
        // Only check grid occupancy once newY is on-screen (avoids negative index UB)
        if (newY >= 0 && grid[newY][newX] == 1) {
          return false;
        }
      }
    }
  }
  return true;
}

void renderGridAndPiece() {
  for (byte i = 0; i < gridHeight; i++) {
    for (byte j = 0; j < gridWidth; j++) {
      gamer.display[j][i] = (grid[i][j] == 1) ? 1 : 0;
    }
  }
  for (byte i = 0; i < 3; i++) {
    for (byte j = 0; j < 3; j++) {
      if (currentPiece[i][j] == 1) {
        int x = currentX + j;
        int y = currentY + i;
        if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
          gamer.display[x][y] = 1;
        }
      }
    }
  }
  gamer.updateDisplay();
}

void createPiece() {
  for (byte i = 0; i < 3; i++)
    for (byte j = 0; j < 3; j++)
      currentPiece[i][j] = 0;
  currentPieceType = (PieceType)random(0, 7);
  switch (currentPieceType) {
    case I:
      currentPiece[1][0] = 1; currentPiece[1][1] = 1; currentPiece[1][2] = 1;
      break;
    case O:
      currentPiece[0][0] = 1; currentPiece[0][1] = 1;
      currentPiece[1][0] = 1; currentPiece[1][1] = 1;
      break;
    case T:
      currentPiece[0][1] = 1;
      currentPiece[1][0] = 1; currentPiece[1][1] = 1; currentPiece[1][2] = 1;
      break;
    case S:
      currentPiece[0][1] = 1; currentPiece[0][2] = 1;
      currentPiece[1][0] = 1; currentPiece[1][1] = 1;
      break;
    case Z:
      currentPiece[0][0] = 1; currentPiece[0][1] = 1;
      currentPiece[1][1] = 1; currentPiece[1][2] = 1;
      break;
    case J:
      currentPiece[0][0] = 1;
      currentPiece[1][0] = 1; currentPiece[1][1] = 1; currentPiece[1][2] = 1;
      break;
    case L:
      currentPiece[0][2] = 1;
      currentPiece[1][0] = 1; currentPiece[1][1] = 1; currentPiece[1][2] = 1;
      break;
  }
  if (soundEnabled) { gamer.playTone(NOTE_G8); tetrisChirpPending = true; }
  startLEDFlash();
}

void checkLines() {
  for (byte i = 0; i < gridHeight; i++) {
    bool lineComplete = true;
    for (byte j = 0; j < gridWidth; j++) {
      if (grid[i][j] == 0) { lineComplete = false; break; }
    }
    if (lineComplete) {
      linesSinceLastDrop++;
      if (soundEnabled) { gamer.playTone(NOTE_A8); tetrisChirpPending = true; }
      startLEDFlash();
      currentX = 3;
      currentY = -1;
      for (byte j = 0; j < gridWidth; j++) {
        grid[i][j] = 0;
        renderGridAndPiece();
        delay(25);
      }
      for (byte k = i; k > 0; k--) {
        for (byte j = 0; j < gridWidth; j++) {
          grid[k][j] = grid[k - 1][j];
        }
      }
      for (byte j = 0; j < gridWidth; j++) grid[0][j] = 0;
      linesCleared += linesSinceLastDrop;
      score += level;
      if (linesCleared % 7 == 0) {
        level++;
        moveInterval = max(300, moveInterval - 200);
      }
    }
  }
}

void rotatePiece() {
  if (currentPieceType == O) return;
  int temp[3][3] = {0};
  for (byte i = 0; i < 3; i++)
    for (byte j = 0; j < 3; j++)
      temp[j][2 - i] = currentPiece[i][j];
  if (canMove(currentX, currentY, temp)) {
    for (byte i = 0; i < 3; i++)
      for (byte j = 0; j < 3; j++)
        currentPiece[i][j] = temp[i][j];
  }
}

int digitFrom(int number, int position) {
  if (position < 1 || number == 0) return 0;
  if (number > 99) number = 99;
  // Replace pow() with simple division
  int divisor = (position == 1) ? 1 : 10;
  int digit = (number / divisor) % 10;
  return digit;
}

void resetTetris() {
  score = 0;
  level = 1;
  linesCleared = 0;
  moveInterval = 1000;
  gameOverT = false;
  currentX = 3;
  currentY = -1;
  for (byte i = 0; i < gridHeight; i++) {
    for (byte j = 0; j < gridWidth; j++) {
      grid[i][j] = 0;
    }
    renderGridAndPiece();
  }
  createPiece();
}

void tetrisLoop() {
  checkSoundToggle();
  updateLEDFlash();

  if (soundEnabled && tetrisChirpPending) {
    gamer.stopTone();
    tetrisChirpPending = false;
  }

  if (soundEnabled) {
    static unsigned long lastNoteTime = 0;
    static byte noteIdx = 0;
    if (millis() - lastNoteTime >= 200) {
      gamer.playTone(pgm_read_byte(&tetrisMelody_pgm[noteIdx]));
      noteIdx = (noteIdx + 1) % TETRIS_MELODY_LEN;
      lastNoteTime = millis();
    }
  }

  if (gameOverT) {
    playLossTune();
    saveHighScore(score, _gameNum);
    showScore(digitFrom(score, 2), digitFrom(score, 1));
    delay(2000);
    resetTetris();
    return;
  }

  static unsigned long lastMoveTime = 0;
  if (millis() - lastMoveTime > moveInterval) {
    lastMoveTime = millis();
    if (canMove(currentX, currentY + 1)) {
      currentY++;
      linesSinceLastDrop = 0;
      startLEDFlash();
      lastMoveTime = millis();
      renderGridAndPiece();
    } else {
      for (byte i = 0; i < 3; i++) {
        for (byte j = 0; j < 3; j++) {
          if (currentPiece[i][j] == 1) {
            int x = currentX + j;
            int y = currentY + i;
            if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
              grid[y][x] = 1;
            }
          }
        }
      }
      if (soundEnabled) { gamer.playTone(NOTE_C8); tetrisChirpPending = true; }
      checkLines();
      createPiece();
      currentX = 3;
      currentY = -3;
      if (!canMove(currentX, currentY) || !canMove(currentX, currentY + 2)) {
        gameOverT = true;
      }
      delay(80);
      renderGridAndPiece();
      currentY = -1;
    }
  }

  bool tetrisBtnPressed = false;
  if(gamer.isPressed(LEFT) && canMove(currentX - 1, currentY)) {
    currentX--;
    isInDownPress = false;
    renderGridAndPiece();
  } else if(gamer.isPressed(RIGHT) && canMove(currentX + 1, currentY)) {
    currentX++;
    isInDownPress = false;
    renderGridAndPiece();
  } else if(gamer.isPressed(DOWN) && canMove(currentX, currentY + 1)) {
    currentY++;
    tetrisBtnPressed = true;
    renderGridAndPiece();
    isInDownPress = true;
    lastDownPressTime = millis();
  } else if(gamer.isHeld(DOWN) && isInDownPress && millis() - lastDownPressTime >= 300) {
    if (canMove(currentX, currentY + 1)) {
      currentY++;
      tetrisBtnPressed = true;
      renderGridAndPiece();
      lastDownPressTime = millis();
    }
  } else if(gamer.isPressed(UP)) {
    isInDownPress = false;
    rotatePiece();
    renderGridAndPiece();
  } else {
    isInDownPress = false;
  }
  if (tetrisBtnPressed) startLEDFlash();
  if (soundEnabled && tetrisBtnPressed) { gamer.playTone(NOTE_D8); tetrisChirpPending = true; }
}

#endif // TETRIS_H
