// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

#ifndef SNAKE_H
#define SNAKE_H

// ── Snake game state ──────────────────────────────────────────────────────────
// currentX, currentY, score declared in main INO
int dir = 1;
byte goalX = 0;
byte goalY = 0;
byte snakeMap[8][8];
byte snakeLength = 2;

void setupSnakeGame() {
  if (startFromHighScore && startingHighScore > 0) {
    // Start with half the high score
    score = startingHighScore / 4;
    snakeLength = min(6, 2 + score);
  } else {
    snakeLength = 2;
    score = 0;
  }
  dir = 1;
  currentX = 0;
  currentY = 0;
  for(byte x=0;x<8;x++) {
    for(byte y=0;y<8;y++) {
      snakeMap[x][y] = 0;
    }
  }
  // Place food on an empty cell (snake starts at 0,0 so avoid that)
  do {
    goalX = random(0,8);
    goalY = random(0,8);
  } while (goalX == 0 && goalY == 0);
  gamer.updateDisplay();
}

void isCollected() {
  if(currentX==goalX && currentY==goalY) {
    // Regenerate food; retry until it lands on an empty cell
    do {
      goalX = random(0,8);
      goalY = random(0,8);
    } while (snakeMap[goalX][goalY] > 0);
    snakeLength++;
    score = snakeLength - 2;
    if (soundEnabled) gamer.playTone(NOTE_A8);
    startLEDFlash();
    for(byte x=0;x<8;x++) {
      for(byte y=0;y<8;y++) {
        snakeMap[x][y]++;
      }
    }
  } else {
    gamer.display[goalX][goalY] = HIGH;
  }
}

void collided() {
  if (snakeMap[currentX][currentY] > 0) {
    gamer.clear();
    delay(20);
    playLossTune();
    saveHighScore(score, 0);
    showScore(score / 10, score % 10);
    delay(800);
    setupSnakeGame();
  }
}

void snakeRec() {
  for(byte x=0;x<8;x++) {
    for(byte y=0;y<8;y++) {
      if(snakeMap[x][y] > 0) {
        snakeMap[x][y]--;
      }
    }
  }
  collided();
  snakeMap[currentX][currentY] = snakeLength;
  for(byte x=0;x<8;x++) {
    for(byte y=0;y<8;y++) {
      if(snakeMap[x][y] > 0) {
        gamer.display[x][y] = HIGH;
      }
    }
  }
}

void snakeLoop() {
  updateGameInput();
  // Clear display manually (faster than gamer.clear() which also calls updateDisplay)
  for(byte x=0;x<8;x++) {
    for(byte y=0;y<8;y++) {
      gamer.display[x][y] = LOW;
    }
  }

  bool snakeBtnPressed = false;
  if(gamer.isPressed(UP) && dir!=3) { dir=1; snakeBtnPressed=true; }
  if(gamer.isPressed(RIGHT) && dir!=4) { dir=2; snakeBtnPressed=true; }
  if(gamer.isPressed(DOWN) && dir!=1) { dir=3; snakeBtnPressed=true; }
  if(gamer.isPressed(LEFT) && dir!=2) { dir=4; snakeBtnPressed=true; }
  if (soundEnabled && snakeBtnPressed) gamer.playTone(NOTE_E8);

  if(dir==1) {
    currentY--;
    if(currentY<0) currentY=7;
  } else if(dir==2) {
    currentX++;
    if(currentX>7) currentX=0;
  } else if(dir==3) {
    currentY++;
    if(currentY>7) currentY=0;
  } else if(dir==4) {
    currentX--;
    if(currentX<0) currentX=7;
  }
  gamer.display[currentX][currentY] = HIGH;
  snakeRec();
  isCollected();
  delay(80);
  gamer.updateDisplay();
}

#endif // SNAKE_H
