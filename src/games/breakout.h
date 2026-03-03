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

void physics() {
  if(gamer.display[currentXBreakout+velocity[0]][currentYBreakout+velocity[1]]==HIGH || outOfBounds(currentXBreakout+velocity[0],currentYBreakout+velocity[1])) {
    //Collided with something!!!
    startLEDFlash();
    if(soundEnabled) {
      if(currentYBreakout==6) {
        gamer.playTone(NOTE_C8);
      } 
      else {
        gamer.playTone(NOTE_E8);
      }
    }
    if(velocity[0]==1) {
      if(velocity[1]==1) {
        if(gamer.display[currentXBreakout+velocity[0]][currentYBreakout-1]==LOW && !outOfBounds(currentXBreakout+velocity[0],currentYBreakout-1)) {
          velocity[1]=-1;
        }
        else if(gamer.display[currentXBreakout-1][currentYBreakout-1]==LOW && !outOfBounds(currentXBreakout-1,currentYBreakout-1)) {
          velocity[1]=-1;
          velocity[0]=-1;
        }
      }
      else if(velocity[1]==-1) {
        if(gamer.display[currentXBreakout+velocity[0]][currentYBreakout+1]==LOW && !outOfBounds(currentXBreakout+velocity[0],currentYBreakout+1)) {
          velocity[1]=1;
        } 
        else if(gamer.display[currentXBreakout-1][currentYBreakout+1]==LOW && !outOfBounds(currentXBreakout-1,currentYBreakout+1)) {
          velocity[1]=1;
          velocity[0]=-1;
        } 
      } 
    } 
    else if(velocity[0]==-1) {
      if(velocity[1]==1) {
        if(gamer.display[currentXBreakout+velocity[0]][currentYBreakout-1]==LOW && !outOfBounds(currentXBreakout+velocity[0],currentYBreakout-1)) {
          velocity[1]=-1;
        } 
        else if(gamer.display[currentXBreakout+1][currentYBreakout-1]==LOW && !outOfBounds(currentXBreakout-1,currentYBreakout-1)) {
          velocity[1]=-1;
          velocity[0]=1;
        }
      } 
      else if(velocity[1]==-1) {
        if(gamer.display[currentXBreakout+velocity[0]][currentYBreakout+1]==LOW && !outOfBounds(currentXBreakout+velocity[0],currentYBreakout+1)) {
          velocity[1]=1;
        } 
        else if(gamer.display[currentXBreakout+1][currentYBreakout+1]==LOW && !outOfBounds(currentXBreakout-1,currentYBreakout+1)) {
          velocity[1]=1;
          velocity[0]=1;
        }
      } 
    }
    if(!outOfBounds(currentXBreakout+origXV,currentYBreakout+origYV)) {
      blocks[currentXBreakout+origXV][currentYBreakout+origYV]=0;
    }
  }
}

void startBreakout(bool resetIt) {
  for(int x=0;x<8;x++) {
    for(int y=0;y<4;y++) {
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
  checkSoundToggle();
  updateLEDFlash();
  if(soundEnabled) gamer.stopTone();
  if(breakoutCounter>2) {
    for(int x=0;x<8;x++) {
      for(int y=0;y<8;y++) {
        gamer.display[x][y] = LOW;
      }
    }
  }
  for(int x=0;x<8;x++) {
    gamer.display[x][7]=LOW;
  }
  if(gamer.isHeld(LEFT) && paddleX>-3) {
    paddleX--;
  } 
  else if(gamer.isHeld(RIGHT) && paddleX<7) {
    paddleX++;
  }
  for(int a=0;a<4;a++) {
    int px = paddleX + a;
    if(px >= 0 && px < 8) {
      gamer.display[px][7] = HIGH;
    }
  }
  if(breakoutCounter>2) {
    origXV = velocity[0];
    origYV = velocity[1];
    for(int x=0;x<8;x++) {
      for(int y=0;y<4;y++) {
        if(blocks[x][y] == 1) {
          gamer.display[x][y] = HIGH;
        }
      }
    }
    physics();
    for(int x=0;x<8;x++) {
      for(int y=0;y<8;y++) {
        if(blocks[x][y]==0) {
          if(x%2==0) {
            if(y%2==0) {
              if(x<7) {
                blocks[x+1][y]=0;
              } else {
                blocks[0][y]=0;
              }
            } 
            else {
              if(x>0) {
                blocks[x-1][y]=0;
              } else {
                blocks[7][y]=0;
              }
            }
          } 
          else {
            if(y%2==0) {
              if(x>0) {
                blocks[x-1][y]=0;
              } else {
                blocks[7][y]=0;
              }
            } 
            else {
              if(x<7) {
                blocks[x+1][y]=0;
              } else {
                blocks[0][y]=0;
              }
            }
          }
        }  
      }
    }
    for(int x=0;x<8;x++) {
      for(int y=0;y<4;y++) {
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
    for(int b=0;b<4;b++) {
      gamer.clear();
      delay(150);
      gamer.display[currentXBreakout][currentYBreakout]=HIGH;
      gamer.updateDisplay();
      delay(150);
    }
    gamer.clear();
    if(scoreBreakout > 0) {
      showScore(scoreBreakout / 10, scoreBreakout % 10);
      saveHighScore(scoreBreakout, 1);
    }
    delay(500);
    startBreakout(true);
  }
  bool finished = true;
  for(int x=0;x<8;x++) {
    for(int y=0;y<4;y++) {
      if(blocks[x][y]==HIGH) finished=false;
    }
  }
  if(finished) {
    startBreakout(false);
  }
  delay(50);
}

#endif // BREAKOUT_H
