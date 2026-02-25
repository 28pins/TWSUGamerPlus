#ifndef SNAKE_H
#define SNAKE_H

// ── Snake game state ──────────────────────────────────────────────────────────
// currentX, currentY, score declared in main INO
int dir = 1;
byte goalX = random(0,7);
byte goalY = random(0,7);
byte snakeMap[8][8];
byte snakeLength = 2;

void setupSnakeGame() {
  snakeLength = 2;
  score = 0;
  dir = 1;
  goalX = random(0,7);
  goalY = random(0,7);
  currentX = 0;
  currentY = 0;
  for(byte x=0;x<8;x++) {
    for(byte y=0;y<8;y++) {
      snakeMap[x][y] = 0;
    }
  }
  gamer.updateDisplay();
}

void isCollected() {
  if(currentX==goalX && currentY==goalY) {
    goalX = random(0,7);
    goalY = random(0,7);
    snakeLength++;
    score++;
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
  for(byte x=0;x<8;x++) {
    for(byte y=0;y<8;y++) {
      if(snakeMap[x][y] > 0) {
        if(currentX == x && currentY == y) {
          gamer.clear();
          delay(20);
          playLossTune();
          saveHighScore((byte)min(score, 99));
          byte dig2 = score % 10;
          byte dig1 = score / 10;
          showScore(dig1,dig2);
          delay(300);
          setupSnakeGame();
        }
      }
    }
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
  checkSoundToggle();
  updateLEDFlash();
  if (soundEnabled) gamer.stopTone();
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
  if (snakeBtnPressed) startLEDFlash();

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
