#ifndef TETRIS_H
#define TETRIS_H

// ── Tetris game state ─────────────────────────────────────────────────────────
// score, currentX, currentY declared in main INO
unsigned long moveInterval = 1000;
volatile int level = 1;
volatile int linesCleared = 0;
volatile bool gameOverT = false;
const int gridWidth = 8;
const int gridHeight = 8;
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

bool canMove(int x, int y, int piece[3][3] = currentPiece) {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (piece[i][j] == 1) {
        int newX = x + j;
        int newY = y + i;
        if (newX < 0 || newX >= gridWidth || newY < -3 || newY >= gridHeight || grid[newY][newX] == 1) {
          return false;
        }
      }
    }
  }
  return true;
}

void renderGridAndPiece() {
  for (int i = 0; i < gridHeight; i++) {
    for (int j = 0; j < gridWidth; j++) {
      gamer.display[j][i] = (grid[i][j] == 1) ? 1 : 0;
    }
  }
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
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
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
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
  for (int i = 0; i < gridHeight; i++) {
    bool lineComplete = true;
    for (int j = 0; j < gridWidth; j++) {
      if (grid[i][j] == 0) { lineComplete = false; break; }
    }
    if (lineComplete) {
      if (soundEnabled) { gamer.playTone(NOTE_A8); tetrisChirpPending = true; }
      startLEDFlash();
      currentX = 3;
      currentY = -1;
      for (int j = 0; j < gridWidth; j++) {
        grid[i][j] = 0;
        renderGridAndPiece();
        delay(30);
      }
      for (int k = i; k > 0; k--) {
        for (int j = 0; j < gridWidth; j++) {
          grid[k][j] = grid[k - 1][j];
        }
      }
      for (int j = 0; j < gridWidth; j++) grid[0][j] = 0;
      linesCleared++;
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
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      temp[j][2 - i] = currentPiece[i][j];
  if (canMove(currentX, currentY, temp)) {
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
        currentPiece[i][j] = temp[i][j];
  }
}

int digitFrom(int number, int position) {
  if (position < 1) return 0;
  if (number == 0) return 0;
  if (number > 99) number = 99;
  int digit = (number / (int)pow(10, position - 1)) % 10;
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
  for (int i = 0; i < gridHeight; i++) {
    for (int j = 0; j < gridWidth; j++) {
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
    static const int melody[] = {NOTE_E8, NOTE_B7, NOTE_C8, NOTE_D8, NOTE_D8, NOTE_C8, NOTE_B7, NOTE_C8,
                                  NOTE_E8, NOTE_A8, NOTE_A8, NOTE_C8, NOTE_E8, NOTE_D8, NOTE_C8, NOTE_B7};
    const byte melodyLen = sizeof(melody) / sizeof(melody[0]);
    if (millis() - lastNoteTime >= 200) {
      gamer.playTone(melody[noteIdx]);
      noteIdx = (noteIdx + 1) % melodyLen;
      lastNoteTime = millis();
    }
  }

  if (gameOverT) {
    playLossTune();
    showScore(digitFrom(score, 2), digitFrom(score, 1));
    delay(3000);
    resetTetris();
    return;
  }

  static unsigned long lastMoveTime = 0;
  if (millis() - lastMoveTime > moveInterval) {
    lastMoveTime = millis();
    if (canMove(currentX, currentY + 1)) {
      currentY++;
      startLEDFlash();
      lastMoveTime = millis();
      renderGridAndPiece();
    } else {
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
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
      delay(100);
      renderGridAndPiece();
      currentY = -1;
    }
  }

  bool tetrisBtnPressed = false;
  if(gamer.isPressed(LEFT) && canMove(currentX - 1, currentY)) {
    currentX--;
    tetrisBtnPressed = true;
    renderGridAndPiece();
  } else if(gamer.isPressed(RIGHT) && canMove(currentX + 1, currentY)) {
    currentX++;
    tetrisBtnPressed = true;
    renderGridAndPiece();
  } else if(gamer.isPressed(DOWN) && canMove(currentX, currentY + 1) && !gamer.isHeld(DOWN)) {
    currentY++;
    tetrisBtnPressed = true;
    renderGridAndPiece();
  } else if(gamer.isHeld(DOWN)) {
    tetrisBtnPressed = true;
    while(canMove(currentX, currentY + 1)) currentY++;
    renderGridAndPiece();
    while(gamer.isHeld(DOWN)) delay(10);
  } else if(gamer.isPressed(UP)) {
    rotatePiece();
    tetrisBtnPressed = true;
    renderGridAndPiece();
  }
  if (tetrisBtnPressed) startLEDFlash();
  if (soundEnabled && tetrisBtnPressed) { gamer.playTone(NOTE_D8); tetrisChirpPending = true; }
}

#endif // TETRIS_H
