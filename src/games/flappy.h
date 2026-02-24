#ifndef FLAPPY_H
#define FLAPPY_H

// ── Flappy Bird game state ────────────────────────────────────────────────────
boolean menu = true;
boolean gameOver = false;
boolean displayflappyScore = false;

int birdPos = 2;
int pipePos = 8;
int pipeGap = 3;
int ticks = 0;
byte flappyScore = 0;
byte inGameScreen[] = {
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
};

byte menuScreen[] = {
  0,0,0,1,1,0,0,0,
  0,0,1,0,0,1,0,0,
  0,0,1,0,1,1,0,0,
  1,1,1,0,0,1,0,0,
  1,0,0,1,1,1,1,0,
  1,0,1,0,1,0,0,1,
  0,1,0,0,1,1,1,0,
  0,0,1,1,1,0,0,0,
};

void drawInGameScreen( byte colour )
{
  if( birdPos >= 0 && birdPos < 8 )
  {
    if( !gameOver || (gameOver && ((ticks / 24) % 2) == 1) )
    {
      inGameScreen[ 1 + birdPos*8 ] = colour;
    }
  }
  for( int y=0; y<8; ++y )
  {
    if( y < pipeGap || y >= pipeGap + 3 )
    {
      if( pipePos >= 0 && pipePos < 8 )
      {
        inGameScreen[ pipePos + y*8 ] = colour;
      }
      if( pipePos >= -1 && pipePos < 7 )
      {
        inGameScreen[ pipePos + 1 + y*8 ] = colour;
      }
    }
  }
}

void resetFlappy(){
  flappyScore = 0;
  displayflappyScore = false;
  ticks = 0;
  birdPos = 2;
  pipePos = 20;
  pipeGap = 3;
  gameOver = false;
  drawInGameScreen( 1 );
}

// Wrapper for launcher: sets menu=true then resets game state
inline void resetFlappyLauncher() {
  menu = true;
  resetFlappy();
}

void flappyLoop()
{
  checkSoundToggle();
  updateLEDFlash();
  if (soundEnabled) gamer.stopTone();
  if( menu )
  {
    ++ticks;
    if( (ticks % 12) == 0 )
    {
      if( rand()%30 == 0 )
      {
        menuScreen[ 19 ] = 1;
        menuScreen[ 20 ] = 0;
      }else
      {
        menuScreen[ 19 ] = 0;
        menuScreen[ 20 ] = 1;
      }
    }

    if(gamer.isPressed(UP)) {
      menu = false;
      resetFlappy();
    }
  }else if(displayflappyScore){
    saveHighScore(flappyScore);
    gamer.clear();
    byte dig2 = flappyScore % 10;
    byte dig1 = (flappyScore-dig2)/10;
    showScore(dig1,dig2);
    delay(1000);
    displayflappyScore = false;
    for(int i = 0 ; i < 64; i++) inGameScreen[i] = 0;
    resetFlappy();
    menu = true;
  }else{
    drawInGameScreen( 0 );

    ++ticks;
    if( !gameOver && ((ticks % 12) == 0) )
    {
      byte lastPipePos = pipePos;
      pipePos--;
      if( pipePos < -1 )
      {
        flappyScore++;
        startLEDFlash();
        pipePos = 7;
        pipeGap = 1 + rand()%4;
      }

      byte lastBirdPos = birdPos;
      if(gamer.isPressed(UP)) {
        birdPos = max( birdPos - 1, 0 );
        if (soundEnabled) gamer.playTone(NOTE_A8);
      }
      else{
        birdPos++;
        if( birdPos >= 8 )
        {
          gameOver = true;
          playLossTune();
          pipePos = lastPipePos;
          birdPos = lastBirdPos;
          ticks = 0;
        }
      }

      if( (pipePos == 1 || pipePos == 0) && (birdPos < pipeGap || birdPos >= pipeGap + 3) )
      {
        gameOver = true;
        playLossTune();
        pipePos = lastPipePos;
        birdPos = lastBirdPos;
        ticks = 0;
      }
    }else if( gameOver && ticks >= 96 ) displayflappyScore = true;
    if( !menu ) drawInGameScreen( 1 );
  }

  if(!displayflappyScore){
    byte* screen = menu ? menuScreen : inGameScreen;
    for(int i = 0 ; i < 64; i++){
      int x = i%8;
      int y = i/8;
      gamer.display[x][y] = screen[i];
    }
    gamer.updateDisplay();
  }
  delay(12);
}

#endif // FLAPPY_H
