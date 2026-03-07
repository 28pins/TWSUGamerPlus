// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

#ifndef BREAKOUT_H
#define BREAKOUT_H
int currentXBreakout = 5;
int currentYBreakout = 5;
int velocity[2] = {
  -1,-1};
byte blocks[8][8];
int paddleX = 2;
byte breakoutCounter = 0;
int origXV=-1;
int origYV=-1;
byte scoreBreakout = 0;
 
bool outOfBounds(int xV, int yV) {
  return (xV > 8 || xV < 0 || yV > 8 || yV < 0);
}

// Helper: checks if a position is free (LOW) and in bounds
inline bool isFree(int x, int y) {
  return !outOfBounds(x, y) && gamer.display[x][y] == LOW;
}

void physics() {
  int nextX = currentXBreakout + velocity[0];
  int nextY = currentYBreakout + velocity[1];

  // Check if we hit something at the next position
  if(gamer.display[nextX][nextY] == HIGH || outOfBounds(nextX, nextY)) {
    // Collision detected!
    startLEDFlash();
    if(soundEnabled) {
      gamer.playTone((currentYBreakout == 6) ? NOTE_C8 : NOTE_E8);
    }

    // Try to bounce off edges intelligently
    // Check if we can bounce just horizontally or just vertically
    bool canBounceY = isFree(nextX, currentYBreakout - velocity[1]);
    bool canBounceX = isFree(currentXBreakout - velocity[0], nextY);

    if(canBounceY) {
      velocity[1] *= -1;  // Bounce vertically
    } else if(canBounceX) {
      velocity[0] *= -1;  // Bounce horizontally
    } else {
      // Corner hit - bounce both directions
      velocity[0] *= -1;
      velocity[1] *= -1;
    }

    // Clear the block we hit (if in bounds)
    if(!outOfBounds(currentXBreakout + origXV, currentYBreakout + origYV)) {
      blocks[currentXBreakout + origXV][currentYBreakout + origYV] = 0;
    }
  }
}

void startBreakout(bool resetIt) {
  for(byte x=0;x<8;x++) {
    for(byte y=0;y<4;y++) {
      blocks[x][y] = 1;
    }
  }
  if(!resetIt) {
    scoreBreakout++;
    gamer.clear();
    saveHighScore(scoreBreakout, 1);
    delay(500);
  } else {
    if (startFromHighScore && startingHighScore > 0) {
      // Start with half the high score
      scoreBreakout = startingHighScore / 4;
    } else {
      scoreBreakout = 0;
    }
  }
  currentXBreakout=random(4,8);
  currentYBreakout=5;
  velocity[0]=-1;
  velocity[1]=-1;
}

void startBreakoutReset() {
  startBreakout(true);
}

void breakoutLoop() {
  updateGameInput();
  if(breakoutCounter>2) {
    for(byte x=0;x<8;x++) {
      for(byte y=0;y<8;y++) {
        gamer.display[x][y] = LOW;
      }
    }
  }
  for(byte x=0;x<8;x++) {
    gamer.display[x][7]=LOW;
  }
  if(gamer.isHeld(LEFT) && paddleX>-3) {
    paddleX--;
  } 
  else if(gamer.isHeld(RIGHT) && paddleX<7) {
    paddleX++;
  }
  for(byte a=0;a<4;a++) {
    int px = paddleX + a;
    if(px >= 0 && px < 8) {
      gamer.display[px][7] = HIGH;
    }
  }
  if(breakoutCounter>2) {
    origXV = velocity[0];
    origYV = velocity[1];
    for(byte x=0;x<8;x++) {
      for(byte y=0;y<4;y++) {
        if(blocks[x][y] == 1) {
          gamer.display[x][y] = HIGH;
        }
      }
    }
    physics();
    // Propagate block destruction to adjacent blocks in a checkerboard pattern
    for(byte x=0;x<8;x++) {
      for(byte y=0;y<8;y++) {
        if(blocks[x][y]==0) {
          // Determine adjacent block based on checkerboard pattern
          byte adjX = ((x % 2) == (y % 2)) ? ((x < 7) ? x + 1 : 0) : ((x > 0) ? x - 1 : 7);
          blocks[adjX][y] = 0;
        }
      }
    }
    for(byte x=0;x<8;x++) {
      for(byte y=0;y<4;y++) {
        if(blocks[x][y] == 0) {
          gamer.display[x][y] = LOW;
        }
      }
    }
    byte newX = currentXBreakout + velocity[0];
    byte newY = currentYBreakout + velocity[1];
    if(newX>-1 && newX<8) {
      if(newY>-1 && newY<8) {
      } 
      else {
        if(gamer.display[newX][currentYBreakout-velocity[1]]==LOW) {
          blocks[newX][currentYBreakout+velocity[1]]=0;
          velocity[1]*=-1;
        } 
        else {
          blocks[currentXBreakout+velocity[0]][currentYBreakout+velocity[1]]=0;
          velocity[1]*=-1;
          velocity[0]*=-1;
        }
      }
    } 
    else {
      if(gamer.display[currentXBreakout-velocity[0]][newY]==LOW) {
        blocks[currentXBreakout+velocity[0]][newY]=0;
        velocity[0]*=-1;
        if(newY<0 || newY>7) {
          if(gamer.display[currentXBreakout+velocity[0]][currentYBreakout-velocity[1]]==LOW) {
            blocks[currentXBreakout-velocity[0]][currentYBreakout-velocity[1]]=0;
            velocity[1]*=-1;
          } 
        }
      } 
      else {
        for(int x=-1;x<2;x++) {
          for(int y=-1;y<2;y++) {
            blocks[currentXBreakout+x][currentYBreakout+y]=0;
          }
        }
        velocity[0]*=-1;
        velocity[1]*=-1;
      }
    }
    currentXBreakout = currentXBreakout+velocity[0];
    currentYBreakout = currentYBreakout+velocity[1];
    gamer.display[currentXBreakout][currentYBreakout] = HIGH;
    breakoutCounter=0;
  } 
  else breakoutCounter++;
  gamer.updateDisplay();
  if(currentYBreakout==7) { //if out of play, lose
    for(byte b=0;b<4;b++) {
      gamer.clear();
      delay(150);
      gamer.display[currentXBreakout][currentYBreakout]=HIGH;
      gamer.updateDisplay();
      delay(150);
    }
    gamer.clear();
    saveHighScore(scoreBreakout, 1);
    if(scoreBreakout > 0) {
      showScore(scoreBreakout / 10, scoreBreakout % 10);
    }
    delay(500);
    startBreakout(true);
  }
  bool finished = true;
  for(byte x=0;x<8;x++) {
    for(byte y=0;y<4;y++) {
      if(blocks[x][y]==HIGH) finished=false;
    }
  }
  if(finished) {
    startBreakout(false);
  }
  delay(50);
}

#endif // BREAKOUT_H
