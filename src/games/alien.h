// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

#ifndef ALIEN_H
#define ALIEN_H
bool gameGoing = false;
long lastMove = 0;
int moveDelay = 1000;

void resetAlienGame() {
  currentX = 3;
  lastMove = millis();
  gameGoing = true;
}

void renderPlayer() {
  for(int i = 0; i < 8; i++) {
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
  for (int i = 0; i < 8; i++) {
    gamer.display[i][0] = random(0, moveDelay > 530 ? 3 : 4) > 1 ? 1 : 0;
  }
}

void moveAlien() {
  for (int i = 0; i < 8; i++) {
    if(gamer.display[i][5] == 1) {
      gameGoing = false;
      saveHighScore(score, _gameNum);
      return;
    }
  }
  for (int i = 0; i < 8; i++) {
    for(int j = 5; j > 0; j--) {
      gamer.display[i][j] = gamer.display[i][j - 1];
    }
  }
  generateAlien();
  renderPlayer();
  gamer.updateDisplay();
  lastMove = millis();
  if(moveDelay > 500) {
    moveDelay -= 5;
  }
}

void alienLoop() {
  if(gameGoing) {
    moveAlien();
    while (millis() - lastMove < moveDelay) {
      delay(10);
      if(gamer.isPressed(UP)){
        for(int i = 7; i >= 0; i--) {
          gamer.display[currentX][i] = 1;
          delay(20);
          renderPlayer();
          gamer.updateDisplay();
        }
        for(int i = 7; i >= 0; i--) {
          gamer.display[currentX][i] = 0;
          delay(20);
          renderPlayer();
          gamer.updateDisplay();
        }
        lastMove += 240;
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
    }
  }
}
#endif