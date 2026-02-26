#ifndef BREAKOUT_H
#define BREAKOUT_H

// ── Breakout game state ───────────────────────────────────────────────────────
int currentXBreakout = 5;
int currentYBreakout = 5;
int velocity[2] = {-1,-1};
byte blocks[8][8];
byte paddleX = 2;
byte breakoutCounter = 0;
int origXV = -1;
int origYV = -1;
byte scoreBreakout = 0;

// Forward declaration needed by startBreakoutReset wrapper below
void startBreakout(bool resetIt);

// Wrapper used by the launcher to reset and start a fresh game
inline void startBreakoutReset() { startBreakout(true); }

bool outOfBounds(int xV, int yV) {
  return (xV >= 8 || xV < 0 || yV >= 8 || yV < 0);
}

void physics() {
  if(gamer.display[currentXBreakout+velocity[0]][currentYBreakout+velocity[1]]==HIGH
     || outOfBounds(currentXBreakout+velocity[0],currentYBreakout+velocity[1])) {
    if(velocity[0]==1) {
      if(velocity[1]==1) {
        if(gamer.display[currentXBreakout+velocity[0]][currentYBreakout-1]==LOW
           && !outOfBounds(currentXBreakout+velocity[0],currentYBreakout-1)) {
          velocity[1]=-1;
        }
        else if(gamer.display[currentXBreakout-1][currentYBreakout-1]==LOW
                && !outOfBounds(currentXBreakout-1,currentYBreakout-1)) {
          velocity[1]=-1;
          velocity[0]=-1;
        }
      }
      else if(velocity[1]==-1) {
        if(gamer.display[currentXBreakout+velocity[0]][currentYBreakout+1]==LOW
           && !outOfBounds(currentXBreakout+velocity[0],currentYBreakout+1)) {
          velocity[1]=1;
        }
        else if(gamer.display[currentXBreakout-1][currentYBreakout+1]==LOW
                && !outOfBounds(currentXBreakout-1,currentYBreakout+1)) {
          velocity[1]=1;
          velocity[0]=-1;
        }
      }
    }
    else if(velocity[0]==-1) {
      if(velocity[1]==1) {
        if(gamer.display[currentXBreakout+velocity[0]][currentYBreakout-1]==LOW
           && !outOfBounds(currentXBreakout+velocity[0],currentYBreakout-1)) {
          velocity[1]=-1;
        }
        else if(gamer.display[currentXBreakout+1][currentYBreakout-1]==LOW
                && !outOfBounds(currentXBreakout-1,currentYBreakout-1)) {
          velocity[1]=-1;
          velocity[0]=1;
        }
      }
      else if(velocity[1]==-1) {
        if(gamer.display[currentXBreakout+velocity[0]][currentYBreakout+1]==LOW
           && !outOfBounds(currentXBreakout+velocity[0],currentYBreakout+1)) {
          velocity[1]=1;
        }
        else if(gamer.display[currentXBreakout+1][currentYBreakout+1]==LOW
                && !outOfBounds(currentXBreakout-1,currentYBreakout+1)) {
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
  for(byte x=0;x<8;x++) {
    for(byte y=0;y<4;y++) {
      blocks[x][y] = 1;
    }
  }
  if(!resetIt) {
    scoreBreakout++;
    gamer.clear();
    byte buf[8]; pgm_readimg(framesBreakout_pgm[0], buf);
    gamer.printImage(buf);
    delay(500);
  } else scoreBreakout=0;
  currentXBreakout=random(4,8);
  currentYBreakout=5;
  velocity[0]=-1;
  velocity[1]=-1;
}

void breakoutLoop() {
  checkSoundToggle();
  updateLEDFlash();
  if(soundEnabled) gamer.stopTone();
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
  if(gamer.isHeld(LEFT)&&paddleX>0){
    paddleX--;
  }
  else if(gamer.isHeld(RIGHT)&&paddleX<4) {
    paddleX++;
  }
  for(byte a=0;a<4;a++) {
    if(paddleX+a<8) {
      gamer.display[paddleX+a][7]=HIGH;
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
    if (velocity[0] != origXV || velocity[1] != origYV) {
      if (soundEnabled) gamer.playTone(currentYBreakout >= 6 ? NOTE_C8 : NOTE_E8);
      startLEDFlash();
    }
    for(byte x=0;x<8;x++) {
      for(byte y=0;y<8;y++) {
        if(blocks[x][y]==0) {
          if((x & 1) == 0) {  // x%2==0
            if((y & 1) == 0) {  // y%2==0
              if(x<7) { blocks[x+1][y]=0; } else { blocks[0][y]=0; }
            }
            else {
              if(x>0) { blocks[x-1][y]=0; } else { blocks[7][y]=0; }
            }
          }
          else {
            if((y & 1) == 0) {  // y%2==0
              if(x>0) { blocks[x-1][y]=0; } else { blocks[7][y]=0; }
            }
            else {
              if(x<7) { blocks[x+1][y]=0; } else { blocks[0][y]=0; }
            }
          }
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
        // when we hit a side wall, always bounce downward
        velocity[1]=1;
        if(newY<0 || newY>7) {
          if(gamer.display[currentXBreakout+velocity[0]][currentYBreakout-velocity[1]]==LOW) {
            blocks[currentXBreakout-velocity[0]][currentYBreakout-velocity[1]]=0;
            // keep the bounce downward
            velocity[1]=1;
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
        // when hitting the wall and clearing surrounding blocks, ensure ball goes downward
        velocity[1]=1;
      }
    }
    currentXBreakout = currentXBreakout+velocity[0];
    currentYBreakout = currentYBreakout+velocity[1];
    gamer.display[currentXBreakout][currentYBreakout] = HIGH;
    breakoutCounter=0;
  }
  else breakoutCounter++;
  gamer.updateDisplay();
  if(currentYBreakout==7) {
    for(byte b=0;b<4;b++) {
      gamer.clear();
      delay(100);
      gamer.display[currentXBreakout][currentYBreakout]=HIGH;
      gamer.updateDisplay();
      delay(100);
    }
    playLossTune();
    saveHighScore(scoreBreakout);
    if(scoreBreakout==0){
      gamer.clear();
      byte buf[8]; pgm_readimg(framesBreakout_pgm[1], buf);
      gamer.printImage(buf);
    }
    else if(scoreBreakout<10){
      gamer.clear();
      showScore(0,scoreBreakout);
    }
    else {
      byte dig2 = scoreBreakout % 10;
      byte dig1 = scoreBreakout / 10;
      gamer.clear();
      showScore(dig1,dig2);
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
    playWinTune();
    startBreakout(false);
  }
  delay(40);
}

#endif // BREAKOUT_H
