// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

#ifndef ALIEN_H
#define ALIEN_H
bool gameGoing = false;
unsigned long lastMove = 0;
unsigned int moveDelay = 800;
int alienLineCount = 0;

void resetAlienGame() {
  gamer.clear();
  if (startFromHighScore && startingHighScore > 0) {
    // Start with half the high score
    score = startingHighScore / 2;
    // alienLineCount increases by 10 per score point
    alienLineCount = score * 10;
    alienLineCount--; // Adjust for initial state
    // Speed increases by 5ms per score point (or ~50 per 10 lines), minimum 300ms
    moveDelay = max(330, 800 - score * 5);
  } else {
    score = 0;
    alienLineCount = 0;
    moveDelay = 800;
  }
  currentX = 3;
  lastMove = millis();
  gameGoing = true;
}

void renderPlayer() {
  for(byte i = 0; i < 8; i++) {
    gamer.display[i][7] = 0;
    gamer.display[i][6] = 0;
  }
  gamer.display[currentX][7] = 1;
  gamer.display[currentX][6] = 1;
  if(currentX > 0) {
    gamer.display[currentX - 1][7] = 1;
  }
  if(currentX < 7) {
    gamer.display[currentX + 1][7] = 1;
  }
}

void generateAlien() {
  for (byte i = 0; i < 8; i++) {
    gamer.display[i][0] = random(0, moveDelay > 330 ? 3 : 4) > 1 ? 1 : 0;
  }
}

void moveAlien() {
  for (byte i = 0; i < 8; i++) {
    if(gamer.display[i][5] == 1) {
      gameGoing = false;
      saveHighScore(score, 5);
      return;
    }
  }
  for (byte i = 0; i < 8; i++) {
    for(byte j = 5; j > 0; j--) {
      gamer.display[i][j] = gamer.display[i][j - 1];
    }
  }
  generateAlien();
  renderPlayer();
  gamer.updateDisplay();
  lastMove = millis();
  alienLineCount++;
  if (alienLineCount % 10 == 0) score++;
  if(moveDelay > 300) {
    moveDelay -= 5;
  }
}

void alienLoop() {
  if(gameGoing) {
    moveAlien();
    while (millis() - lastMove < moveDelay) {
      delay(10);
      updateGameInput();
      if(gamer.isPressed(UP)){
        for(int8_t i = 7; i >= 0; i--) {
          gamer.display[currentX][i] = 1;
          delay(20);
          renderPlayer();
          gamer.updateDisplay();
        }
        for(int8_t i = 7; i >= 0; i--) {
          gamer.display[currentX][i] = 0;
          delay(20);
          renderPlayer();
          gamer.updateDisplay();
        }
        lastMove += 200;
      }
      if(gamer.isPressed(LEFT) && currentX > 0) {
        currentX--;
        renderPlayer();
        gamer.updateDisplay();
      }
      if(gamer.isPressed(RIGHT) && currentX < 7) {
        currentX++;
        renderPlayer();
        gamer.updateDisplay();
      }
    }
  } else {
    if(gamer.isPressed(UP)) {
      resetAlienGame();
    } else if(gamer.isPressed(LEFT) || gamer.isPressed(RIGHT)) {
      resetAlienGame();
    } else {
      showScore(score/10, score%10);
      delay(1000);
      resetAlienGame();
    }
  }
}
#endif