#include "Gamer.h"

// Create a copy of the Gamer library.
Gamer gamer;

byte startup[1][8]; //declare at top of code
byte snake[2][8];  //snake animation
byte breakout[2][8]; //breakout anim
byte simon[2][8]; //simon anim
byte flappy[2][8]; //flappy anim
byte tetris[2][8]; //tetris anim
byte alienAnim[2][8]; //alien (space invaders) anim
byte conwayAnim[2][8]; //conway's game of life anim
volatile byte animationLength[] = { //how long is each animation???
  2,2,2,2,2,2,2};
volatile byte animationFrame = 0; //what frame is it???
volatile byte gameNumber = 0; //what game is it???
volatile byte gameMax = 7; //how many games are there???

// Sound toggle — starts OFF; touching the cap sense pad toggles it
bool soundEnabled = false;
bool lastCapTouchState = false;
bool tetrisChirpPending = false; // true when an event chirp needs explicit stop next iteration

// Non-blocking LED flash: call startLEDFlash() on an event, updateLEDFlash() each loop tick
unsigned long ledFlashStartTime = 0;
bool ledFlashing = false;

void startLEDFlash() {
  gamer.setLED(true);
  ledFlashStartTime = millis();
  ledFlashing = true;
}

void updateLEDFlash() {
  if (ledFlashing && millis() - ledFlashStartTime >= 300UL) {
    gamer.setLED(false);
    ledFlashing = false;
  }
}

// Feature flag: set to 1 to enable the right-arrow direction in Simon.
// Disabled by default because the RIGHT button has a known recognition bug.
#define SIMON_RIGHT_ARROW_ENABLED 0

#if SIMON_RIGHT_ARROW_ENABLED
  #define SIMON_NUM_DIRECTIONS 4
#else
  #define SIMON_NUM_DIRECTIONS 3
#endif

// gamer.playTone() note values: frequency = 1,000,000 / (OCR2A + 1) Hz
#define NOTE_B7  252  // ~3937 Hz
#define NOTE_C8  238  // ~4202 Hz
#define NOTE_D8  212  // ~4717 Hz
#define NOTE_E8  189  // ~5263 Hz
#define NOTE_G8  158  // ~6289 Hz
#define NOTE_A8  140  // ~7042 Hz
#define NOTE_B8  125  // ~7937 Hz

#define WIN_NOTE_DURATION  120  // ms per note in win tune
#define LOSS_NOTE_DURATION 150  // ms per note in loss tune

// Detects a rising edge on the cap sense pad and toggles sound on/off
void checkSoundToggle() {
  bool cap = gamer.capTouch();
  if (cap && !lastCapTouchState) {
    soundEnabled = !soundEnabled;
    if (!soundEnabled) gamer.stopTone();
  }
  lastCapTouchState = cap;
}

// Plays a short ascending tune on win/success events (if sound enabled).
void playWinTune() {
  if (!soundEnabled) return;
  static const PROGMEM byte notes[] = {NOTE_C8, NOTE_E8, NOTE_G8, NOTE_B8};
  for (byte i = 0; i < 4; i++) { gamer.playTone(pgm_read_byte(&notes[i])); delay(WIN_NOTE_DURATION); }
  gamer.stopTone();
}

// Plays a short descending tune on loss/fail events (if sound enabled).
void playLossTune() {
  if (!soundEnabled) return;
  static const PROGMEM byte notes[] = {NOTE_B8, NOTE_G8, NOTE_E8, NOTE_B7};
  for (byte i = 0; i < 4; i++) { gamer.playTone(pgm_read_byte(&notes[i])); delay(LOSS_NOTE_DURATION); }
  gamer.stopTone();
}

void setup() {
  gamer.begin();
  setupLogo(); //starting anim
  setupSnake(); //snake anim
  setupBreakout(); //breakout anim
  setupSimon(); //simon says anim
  setupFlappy(); //flappy anim
  setupTetris(); //tetris anim
  setupAlienAnim(); //alien (space invaders) anim
  setupConwayAnim(); //conway's game of life anim
  setupImages(); //breakout win/lose images
  setupSimonImages(); //simon arrow and result images
  gamer.printImage(startup[0]); delay(100); // startup frame
  // Serial.begin(9600);
  // Serial.println("Setup complete!");
}

void loop() { //selector
  checkSoundToggle();
  if(gamer.isPressed(START)) {
    //start game!
    switch(gameNumber) {
    case 0:
      //snake!
      //wait for start to be released
      setupSnakeGame(); //setup, well, snake
      while(!gamer.isPressed(START)) {
        //run the main game loop!
        snakeLoop();
      }
      gamer.stopTone();
      break;
    case 1:
      startBreakout(true);
      //breakout
      while(!gamer.isPressed(START)) {
        breakoutLoop();
      }
      gamer.stopTone();
      break;
    case 2:
      resetSimon();
      while(!gamer.isPressed(START)) {
        simonLoop();
      }
      gamer.stopTone();
      break;
    case 3:
      menu = true;
      resetFlappy();
      while(!gamer.isPressed(START)) {
        flappyLoop();
      }
      gamer.stopTone();
      break;
    case 4:
      resetTetris();
      while(!gamer.isPressed(START)) {
        tetrisLoop();
      }
      gamer.stopTone();
      break;
    case 5:
      resetAlienGame();
      while(!gamer.isPressed(START)) {
        alienLoop();
      }
      gamer.stopTone();
      break;
    case 6:
      resetConway();
      while(!gamer.isPressed(START)) {
        conwayLoop();
      }
      gamer.stopTone();
      break;
    }
  } 
  else {
    //play animations!
    switch(gameNumber) {
    case 0:
      gamer.printImage(snake[animationFrame]);
      break;
    case 1:
      //breakout
      gamer.printImage(breakout[animationFrame]);
      break;//out
    case 2:
      //simon
      gamer.printImage(simon[animationFrame]);
      break;
    case 3:
      //flappy
      gamer.printImage(flappy[animationFrame]);
      break;
    case 4:
      //tetris
      gamer.printImage(tetris[animationFrame]);
      break;
    case 5:
      //alien (space invaders)
      gamer.printImage(alienAnim[animationFrame]);
      break;
    case 6:
      //conway's game of life
      gamer.printImage(conwayAnim[animationFrame]);
      break;
    }
    animationFrame++;
    if(animationFrame>animationLength[gameNumber]-1) animationFrame=0;
    //now check buttons!
    if(gamer.isPressed(LEFT)) {
      gameNumber--;
      animationFrame=0;
    }
    if(gamer.isPressed(RIGHT)) {
      gameNumber++;
      animationFrame=0;
    }
    if(gameNumber==255) gameNumber=gameMax-1;
    if(gameNumber>=gameMax) gameNumber=0;
    if(gamer.isPressed(UP)) { //toggle sound on/off
      soundEnabled = !soundEnabled;
      if(soundEnabled) {
        gamer.playTone(80);
        delay(150);
        gamer.stopTone();
      }
    }
    delay(300);
  }
}

void setupLogo() { //run this at the start
  startup[0][0] = B11111111;
  startup[0][1] = B11111111;
  startup[0][2] = B11111111;
  startup[0][3] = B11111111;
  startup[0][4] = B11111111;
  startup[0][5] = B11111111;
  startup[0][6] = B11111111;
  startup[0][7] = B11111111;
}

void setupSnake() { //run this at the start
  // Frame 0: S-curve snake + food dot
  snake[0][0] = B00000000;
  snake[0][1] = B00111000; // head+body (cols 2,3,4)
  snake[0][2] = B00001000; // bend down (col 4)
  snake[0][3] = B00001110; // tail goes right (cols 4,5,6)
  snake[0][4] = B00000000;
  snake[0][5] = B01000000; // food dot (col 1)
  snake[0][6] = B00000000;
  snake[0][7] = B00000000;
  // Frame 1: same snake, food dot off (blinking)
  snake[1][0] = B00000000;
  snake[1][1] = B00111000;
  snake[1][2] = B00001000;
  snake[1][3] = B00001110;
  snake[1][4] = B00000000;
  snake[1][5] = B00000000;
  snake[1][6] = B00000000;
  snake[1][7] = B00000000;
}

void setupBreakout() { //run this at the start
  // Frame 0: full block rows, ball mid-field, paddle at bottom
  breakout[0][0] = B11111111; // solid block row
  breakout[0][1] = B11111111; // solid block row
  breakout[0][2] = B00000000;
  breakout[0][3] = B00010000; // ball (col 3)
  breakout[0][4] = B00000000;
  breakout[0][5] = B00000000;
  breakout[0][6] = B00000000;
  breakout[0][7] = B00111000; // paddle (cols 2,3,4)
  // Frame 1: one block broken, ball moved down
  breakout[1][0] = B11111111; // top row intact
  breakout[1][1] = B11011111; // block at col 2 broken
  breakout[1][2] = B00000000;
  breakout[1][3] = B00000000;
  breakout[1][4] = B00010000; // ball dropped one row
  breakout[1][5] = B00000000;
  breakout[1][6] = B00000000;
  breakout[1][7] = B00111000; // paddle (cols 2,3,4)
}

void setupSimon() {
  // Frame 0: top-left + bottom-right quadrants lit
  simon[0][0] = B11110000;
  simon[0][1] = B11110000;
  simon[0][2] = B11110000;
  simon[0][3] = B11110000;
  simon[0][4] = B00001111;
  simon[0][5] = B00001111;
  simon[0][6] = B00001111;
  simon[0][7] = B00001111;
  // Frame 1: top-right + bottom-left quadrants lit
  simon[1][0] = B00001111;
  simon[1][1] = B00001111;
  simon[1][2] = B00001111;
  simon[1][3] = B00001111;
  simon[1][4] = B11110000;
  simon[1][5] = B11110000;
  simon[1][6] = B11110000;
  simon[1][7] = B11110000;
}

void setupFlappy() { //run this at the start
  // Frame 0: bird wing up, pipe with gap at rows 2-5
  flappy[0][0] = B00000001; // pipe (col 7)
  flappy[0][1] = B00000001; // pipe
  flappy[0][2] = B01000000; // bird wing up (col 1), gap
  flappy[0][3] = B01100000; // bird body (cols 1,2), gap
  flappy[0][4] = B00000000; // gap
  flappy[0][5] = B00000000; // gap
  flappy[0][6] = B00000001; // pipe resumes
  flappy[0][7] = B00000001; // pipe
  // Frame 1: bird wing down, same pipe gap
  flappy[1][0] = B00000001; // pipe
  flappy[1][1] = B00000001; // pipe
  flappy[1][2] = B00000000; // gap
  flappy[1][3] = B01100000; // bird body (cols 1,2), gap
  flappy[1][4] = B01000000; // bird wing down (col 1), gap
  flappy[1][5] = B00000000; // gap
  flappy[1][6] = B00000001; // pipe resumes
  flappy[1][7] = B00000001; // pipe
}

void setupTetris() {
  // Frame 0: O-piece at top (rows 0-1), blocks at bottom with gap
  tetris[0][0] = B00110000; // O-piece top (cols 2,3)
  tetris[0][1] = B00110000; // O-piece bottom (cols 2,3)
  tetris[0][2] = B00000000;
  tetris[0][3] = B00000000;
  tetris[0][4] = B00000000;
  tetris[0][5] = B00000000;
  tetris[0][6] = B11001111; // blocks with gap at cols 2,3
  tetris[0][7] = B11111111; // full bottom row
  // Frame 1: O-piece dropped one row (rows 1-2)
  tetris[1][0] = B00000000;
  tetris[1][1] = B00110000; // O-piece top (cols 2,3)
  tetris[1][2] = B00110000; // O-piece bottom (cols 2,3)
  tetris[1][3] = B00000000;
  tetris[1][4] = B00000000;
  tetris[1][5] = B00000000;
  tetris[1][6] = B11001111; // blocks with gap at cols 2,3
  tetris[1][7] = B11111111; // full bottom row
}

//BREAKOUT CODE
volatile int currentXBreakout = 5;
volatile int currentYBreakout = 5;
volatile int velocity[2] = {
  -1,-1};
volatile byte blocks[8][8];
volatile byte paddleX = 2;
volatile byte counter = 0;
volatile int origXV=-1;
volatile int origYV=-1;
volatile byte scoreBreakout = 0;

// NOTE TO SELF: The physics in this is still iffy.
// FIX IT!!!

byte framesBreakout[2][8]; //declare at top of code
void setupImages() { //run this at the start
  framesBreakout[0][0] = B00000000;
  framesBreakout[0][1] = B01100110;
  framesBreakout[0][2] = B01100110;
  framesBreakout[0][3] = B00000000;
  framesBreakout[0][4] = B01000010;
  framesBreakout[0][5] = B00111100;
  framesBreakout[0][6] = B00000000;
  framesBreakout[0][7] = B00000000;
  framesBreakout[1][0] = B00000000;
  framesBreakout[1][1] = B01100110;
  framesBreakout[1][2] = B01100110;
  framesBreakout[1][3] = B00000000;
  framesBreakout[1][4] = B00000000;
  framesBreakout[1][5] = B00111100;
  framesBreakout[1][6] = B01000010;
  framesBreakout[1][7] = B00000000;
}

void startBreakout(boolean resetIt) {
  for(int x=0;x<8;x++) {
    for(int y=0;y<4;y++) {
      blocks[x][y] = 1;
    }
  }
  if(!resetIt) {
    scoreBreakout++;
    gamer.clear();
    gamer.printImage(framesBreakout[0]);
    delay(500);
  } 
  else scoreBreakout=0;
  currentXBreakout=random(4,8);
  currentYBreakout=5;
  velocity[0]=-1;
  velocity[1]=-1;
}

void breakoutLoop() {
  checkSoundToggle();
  updateLEDFlash();
  if (soundEnabled) gamer.stopTone(); // stop previous bounce chirp
  if(counter>2) {
    for(int x=0;x<8;x++) {
      for(int y=0;y<8;y++) {
        gamer.display[x][y] = LOW;
      }
    }
  }
  for(int x=0;x<8;x++) {
    gamer.display[x][7]=LOW;
  }
  if(gamer.isHeld(LEFT)&&paddleX>0){
    paddleX--;
  } 
  else if(gamer.isHeld(RIGHT)&&paddleX<4) {
    paddleX++;
  }
  for(int a=0;a<4;a++) {
    if(paddleX+a<8) {
      gamer.display[paddleX+a][7]=HIGH;
    }
  }
  if(counter>2) {
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
    // Bounce sound and LED flash — plays when velocity changes (ball hits block, wall, or paddle)
    if (velocity[0] != origXV || velocity[1] != origYV) {
      if (soundEnabled) gamer.playTone(currentYBreakout >= 6 ? NOTE_C8 : NOTE_E8); // lower for paddle, higher for blocks
      startLEDFlash(); // flash on every collision
    }
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
    counter=0;
  } 
  else counter++;
  gamer.updateDisplay();
  if(currentYBreakout==7) { //if out of play, lose
    for(int b=0;b<4;b++) {
      gamer.clear();
      delay(150);
      gamer.display[currentXBreakout][currentYBreakout]=HIGH;
      gamer.updateDisplay();
      delay(150);
    }
    playLossTune();
    if(scoreBreakout==0){
      gamer.clear();
      gamer.printImage(framesBreakout[1]);
    } 
    else if(scoreBreakout<10){
      gamer.clear();
      showScore(0,scoreBreakout);
    } 
    else {
      int dig2 = scoreBreakout % 10;  //split scoreBreakout into two digits (eg 10 -> 1 and 0)
      int dig1 = (scoreBreakout-(scoreBreakout%10))/10;
      gamer.clear();
      showScore(dig1,dig2);
    }
    delay(500);
    startBreakout(true);
  }
  boolean finished = true;
  for(int x=0;x<8;x++) {
    for(int y=0;y<4;y++) {
      if(blocks[x][y]==HIGH) finished=false;
    }
  }
  if(finished) {
    playWinTune();
    startBreakout(false);
  }
  delay(50);
}
 
boolean outOfBounds(int xV, int yV) {
  if(xV >= 8 || xV < 0) {
    return true;
  } 
  else if(yV >= 8 || yV < 0) {
    return true;
  } 
  else {
    return false;
  }
}
 
void physics() {
  if(gamer.display[currentXBreakout+velocity[0]][currentYBreakout+velocity[1]]==HIGH || outOfBounds(currentXBreakout+velocity[0],currentYBreakout+velocity[1])) {
    //Collided with something!!!
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
        else if(gamer.display[currentXBreakout+1][currentYBreakout-1]==LOW && !outOfBounds(currentXBreakout+1,currentYBreakout-1)) {
          velocity[1]=-1;
          velocity[0]=1;
        }
      } 
      else if(velocity[1]==-1) {
        if(gamer.display[currentXBreakout+velocity[0]][currentYBreakout+1]==LOW && !outOfBounds(currentXBreakout+velocity[0],currentYBreakout+1)) {
          velocity[1]=1;
        } 
        else if(gamer.display[currentXBreakout+1][currentYBreakout+1]==LOW && !outOfBounds(currentXBreakout+1,currentYBreakout+1)) {
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


//MARK: ADVANCED DISPLAY CODE
const byte numbers[10][8] = {
  { B00000111, B00000101, B00000101, B00000101, B00000101, B00000101, B00000101, B00000111 }, // 0
  { B00000100, B00000100, B00000100, B00000100, B00000100, B00000100, B00000100, B00000100 }, // 1
  { B00000111, B00000001, B00000001, B00000111, B00000100, B00000100, B00000100, B00000111 }, // 2
  { B00000111, B00000001, B00000001, B00000011, B00000001, B00000001, B00000001, B00000111 }, // 3
  { B00000101, B00000101, B00000101, B00000111, B00000001, B00000001, B00000001, B00000001 }, // 4
  { B00000111, B00000100, B00000100, B00000111, B00000001, B00000001, B00000001, B00000111 }, // 5
  { B00000111, B00000100, B00000100, B00000111, B00000101, B00000101, B00000101, B00000111 }, // 6
  { B00000111, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001 }, // 7
  { B00000111, B00000101, B00000101, B00000111, B00000101, B00000101, B00000101, B00000111 }, // 8
  { B00000111, B00000101, B00000101, B00000111, B00000001, B00000001, B00000001, B00000111 }, // 9
};

void showScore(byte dig1,byte dig2) {
  byte result[8];
  for(int p=0;p<8;p++) {
    // shift tens digit into upper 3 bits (cols 7-5), units digit occupies lower 3 bits (cols 2-0)
    result[p]=(numbers[dig1][p]<<5)|numbers[dig2][p];
  }
  gamer.printImage(result);
}

//MARK:FLAPPY code
volatile boolean menu = true;
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


// UTILITY
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
void flappyLoop() 
{  
  checkSoundToggle();
  updateLEDFlash();
  if (soundEnabled) gamer.stopTone(); // stop previous chirp
  // Update
  if( menu )
  {
    // IN MENU
    ++ticks;
    if( (ticks % 12) == 0 )
    {
      // Animate the eye
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
    gamer.clear();
    byte dig2 = flappyScore % 10;  //split flappyScore into two digits (eg 10 -> 1 and 0)
    byte dig1 = (flappyScore-dig2)/10;
    showScore(dig1,dig2);
    delay(1000);
    displayflappyScore = false;
    for(int i = 0 ; i < 64; i++) inGameScreen[i] = 0;
    resetFlappy();
    menu = true;
  }else{
    // IN GAME
    // Clear the screen
    drawInGameScreen( 0 );
    
    // Update the state
    ++ticks;
    if( !gameOver && ((ticks % 12) == 0) )
    {
      // Move the pipe
      byte lastPipePos = pipePos;
      pipePos--;
      if( pipePos < -1 )
      {
        flappyScore++;
        startLEDFlash(); // flash when a gate is passed
        pipePos = 7; 
        pipeGap = 1 + rand()%4;
      }
           
      // Move the bird
      byte lastBirdPos = birdPos;
      if(gamer.isPressed(UP)) {
        birdPos = max( birdPos - 1, 0 );//move the bird upwards when UP key is pressed
        if (soundEnabled) gamer.playTone(NOTE_A8); // wing-flap chirp
      }
      else{
        birdPos++;//move the bird down
        if( birdPos >= 8 )//check if the bird hit the ground
        {
          gameOver = true;
          playLossTune();
          pipePos = lastPipePos;
          birdPos = lastBirdPos;
          ticks = 0;
        }
      }
      
      // Test for pipe collision
      if( (pipePos == 1 || pipePos == 0) && (birdPos < pipeGap || birdPos >= pipeGap + 3) )
      {
        gameOver = true;
        playLossTune();
        pipePos = lastPipePos;
        birdPos = lastBirdPos;
        ticks = 0;
      }
//    }else if( gameOver && ticks >= 96 ) menu = true;//game over, display the last position for 96 ticks then return to menu
    }else if( gameOver && ticks >= 96 ) displayflappyScore = true;//game over, display the last position for 96 ticks then return to menu
    if( !menu ) drawInGameScreen( 1 ); // Redraw the screen
  }
  
  // Render
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

//MARK:SIMON CODE
byte x=0;
int delayMils = 300; //larger = easier
byte framesSimon[4][8];
byte go[8];
byte right[8];
byte wrong[8];
volatile byte sequence[30];

void resetSimon() {
  gamer.clear();
  delay(100);
  for(byte b=0;b<x;b++) sequence[b]=0;
  x=0;
  delayMils = 300;
}

void simonLoop() {
  checkSoundToggle();
  updateLEDFlash();
  // Four distinct tones — one per direction (up, down, left, right)
  static const int simonNotes[] = {NOTE_E8, NOTE_C8, NOTE_G8, NOTE_D8};
  sequence[x]=random(0, SIMON_NUM_DIRECTIONS);
  if(x>0) {
    for(byte p=3;p>0;p--) {
      showScore(0,p);
      delay(delayMils);
    }
    gamer.printImage(go); //move this later...
    delay(delayMils);
    for(int i=0;i<x;i++) {
      if(gamer.isHeld(START)) return;
      if (soundEnabled) gamer.playTone(simonNotes[sequence[i]]);
      gamer.printImage(framesSimon[sequence[i]]);
      startLEDFlash(); // flash when arrow is displayed
      delay(delayMils);
      if (soundEnabled) gamer.stopTone();
      gamer.clear();
      delay(delayMils);
    }
    gamer.clear();
    boolean success = true;
    for(byte count=0;count<x;count++) {
      if(gamer.isHeld(START)) return;
      byte key = 4;
      while(key==4) { //wait for a keypress
        if(gamer.isHeld(START)) return;
        if(gamer.isHeld(UP)) key=0;
        if(gamer.isHeld(DOWN)) key=1;
        if(gamer.isHeld(LEFT)) key=2;
#if SIMON_RIGHT_ARROW_ENABLED
        if(gamer.isHeld(RIGHT)) key=3;
#endif
        while(gamer.isHeld(RIGHT) || gamer.isHeld(LEFT) || gamer.isHeld(UP) || gamer.isHeld(DOWN))
      }
      startLEDFlash(); // flash when player presses a button
      gamer.printImage(framesSimon[key]);
      //is it right???
      if(key!=sequence[count]) {
        success = false; // game over...
        break;
      }
    }
    delayMils-=(delayMils/40);
    delay(delayMils);
    if(success) {
      x++; //they got it right, MAKE IT HARDER!
      playWinTune();
      gamer.printImage(right);
    } 
    else {
      playLossTune();
      gamer.printImage(wrong);
      delay(500);
      showScore((x-1)/10,(x-1)%10); //showScore wants digits, not numbers! (as in 1,5 rather than 15)
      delay(500);
      resetSimon();
    }
  } else {
    x++; //rack up the difficulty anyway!
  }
  delay(500);
}

void setupSimonImages() {
  framesSimon[0][0] = B00000000; //up
  framesSimon[0][1] = B00011000;
  framesSimon[0][2] = B00111100;
  framesSimon[0][3] = B01111110;
  framesSimon[0][4] = B00011000;
  framesSimon[0][5] = B00011000;
  framesSimon[0][6] = B00011000;
  framesSimon[0][7] = B00000000;
  framesSimon[1][7] = B00000000; //down (aka up, but flipped)
  framesSimon[1][6] = B00011000;
  framesSimon[1][5] = B00111100;
  framesSimon[1][4] = B01111110;
  framesSimon[1][3] = B00011000;
  framesSimon[1][2] = B00011000;
  framesSimon[1][1] = B00011000;
  framesSimon[1][0] = B00000000;
  framesSimon[2][0] = B00000000; //left
  framesSimon[2][1] = B00010000;
  framesSimon[2][2] = B00110000;
  framesSimon[2][3] = B01111110;
  framesSimon[2][4] = B01111110;
  framesSimon[2][5] = B00110000;
  framesSimon[2][6] = B00010000;
  framesSimon[2][7] = B00000000;
  framesSimon[3][0] = B00000000; //right (left but flipped)
  framesSimon[3][1] = B00001000;
  framesSimon[3][2] = B00001100;
  framesSimon[3][3] = B01111110;
  framesSimon[3][4] = B01111110;
  framesSimon[3][5] = B00001100;
  framesSimon[3][6] = B00001000;
  framesSimon[3][7] = B00000000;
  go[0] = B00000000;
  go[1] = B01101110;
  go[2] = B10001010;
  go[3] = B10001010;
  go[4] = B10001010;
  go[5] = B10101010;
  go[6] = B01101110;
  go[7] = B00100000;
  wrong[0] = B11000011;
  wrong[1] = B01100110;
  wrong[2] = B00111100;
  wrong[3] = B00011000;
  wrong[4] = B00011000;
  wrong[5] = B00111100;
  wrong[6] = B01100110;
  wrong[7] = B11000011;
  right[0] = B00000001;
  right[1] = B00000011;
  right[2] = B00000111;
  right[3] = B00001110;
  right[4] = B11011100;
  right[5] = B11111000;
  right[6] = B01110000;
  right[7] = B00100000;
}


//MARK:SNAKE CODE
int currentX = 0;
int currentY = 0;
int dir = 1;
byte goalX = random(0,7);
byte goalY = random(0,7);
volatile byte snakeMap[8][8];
byte snakeLength = 2;
int score = 0;

void setupSnakeGame() {
  snakeLength = 2;
  score = 0;
  dir = 1;
  goalX = random(0,7);
  goalY = random(0,7);
  currentX = 0;
  currentY = 0;
  for(int x=0;x<8;x++) {
    for(int y=0;y<8;y++) {
      snakeMap[x][y] = 0;
    }
  }
  gamer.updateDisplay();
}

void snakeLoop() {
  checkSoundToggle();
  updateLEDFlash();
  if (soundEnabled) gamer.stopTone(); // stop previous chirp
  //gamer.clear, but DON'T UPDATE YET!!!!
  for(int x=0;x<8;x++) {
    for(int y=0;y<8;y++) {
      gamer.display[x][y] = LOW;
    }
  }
  //buttons should be here!
  //when upPressed etc. has been added, uncomment this next section and then comment out the random directions section:
  
  bool snakeBtnPressed = false;
  if(gamer.isPressed(UP) && dir!=3) { dir=1; snakeBtnPressed=true; }
  if(gamer.isPressed(RIGHT) && dir!=4) { dir=2; snakeBtnPressed=true; }
  if(gamer.isPressed(DOWN) && dir!=1) { dir=3; snakeBtnPressed=true; }
  if(gamer.isPressed(LEFT) && dir!=2) { dir=4; snakeBtnPressed=true; }
  if (soundEnabled && snakeBtnPressed) gamer.playTone(NOTE_E8);
  if (snakeBtnPressed) startLEDFlash(); // flash on direction button press
  
  //this is a random directions function. comment it out when button support has been added
  //if(random(0,10)>7) dir++;
  //if(dir>4) dir=1;
  //this is the end of a random directions function.
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
  delay(100);
  gamer.updateDisplay();
}

void isCollected() {
  if(currentX==goalX && currentY==goalY) {
    goalX = random(0,7);
    goalY = random(0,7);
    snakeLength++;
    score++;
    if (soundEnabled) gamer.playTone(NOTE_A8); // pentatonic chirp when food is eaten
    startLEDFlash(); // flash when food square is eaten
    for(int x=0;x<8;x++) {
      for(int y=0;y<8;y++) {
        snakeMap[x][y]++;
      }
    }
  } else {
    gamer.display[goalX][goalY] = HIGH;
  }
}

void snakeRec() {
  for(int x=0;x<8;x++) {
    for(int y=0;y<8;y++) {
      if(snakeMap[x][y] > 0) {
        snakeMap[x][y]--;
      }
    }
  }
  collided();
  snakeMap[currentX][currentY] = snakeLength;
  for(int x=0;x<8;x++) {
    for(int y=0;y<8;y++) {
      if(snakeMap[x][y] > 0) {
        gamer.display[x][y] = HIGH;
      }
    }
  }
}

void collided() {
  for(int x=0;x<8;x++) { //it seems to work if I add this :p
    for(int y=0;y<8;y++) {
      if(snakeMap[x][y] > 0) {
        if(currentX == x && currentY == y) {
          gamer.clear();
          delay(20);
          playLossTune();
          //printString("GAME OVER  you scored",40);
          byte dig2 = score % 10;  //split score into two digits (eg 10 -> 1 and 0)
          byte dig1 = (score-(score%10))/10;
          showScore(dig1,dig2);
          delay(300);
          setupSnakeGame();
        }
      }
    }
  }
}

//MARK:TETRIS CODE
//Tetris Game for Arduino
unsigned long moveInterval = 1000; // Initial move interval in milliseconds
volatile int level = 1;
volatile int linesCleared = 0;
volatile bool gameOverT = false;
const int gridWidth = 8;
const int gridHeight = 8;
int grid[gridHeight][gridWidth] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}}; // 0 for empty, 1 for filled
int currentPiece[3][3] = { //empty piece
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};

bool canMove(int x, int y, int piece[3][3] = currentPiece) {
  // Serial.print("Checking canMove for x: ");
  // Serial.print(x);
  // Serial.print(", y: ");
  // Serial.println(y); 
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (piece[i][j] == 1) { // Check only filled blocks
                int newX = x + j;
                int newY = y + i;
                // Check boundaries
                if (newX < 0 || newX >= gridWidth || newY < -3 || newY >= gridHeight || grid[newY][newX] == 1) {
                  // Serial.println("Collision detected or out of bounds!");
                    return false; // Can't move
                }

            }
        }
    }
    // Serial.println("Move is valid.");
    return true; // Can move
}

void resetTetris() {
  // Serial.println("Resetting Tetris...");
    // Initialize the game state
    score = 0;
    level = 1;
    linesCleared = 0;
    moveInterval = 1000;
    gameOverT = false;
    currentX = 3;
    currentY = -1;
    
    // Clear the grid
    for (int i = 0; i < gridHeight; i++) {
        for (int j = 0; j < gridWidth; j++) {
        grid[i][j] = 0;
        }
        renderGridAndPiece();
    }

    createPiece();
}

int digitFrom(int number, int position) {
    if (position < 1) return 0; // Invalid position
    if (number == 0) return 0; // Handle the case for 0
    if (number > 99) number = 99; // Cap the score at 99 for display purposes
    int digit = (number / (int)pow(10, position - 1)) % 10;
    return digit;
}

void tetrisLoop() {
    checkSoundToggle();
    updateLEDFlash();

    // Stop any event chirp that was started in the previous iteration
    if (soundEnabled && tetrisChirpPending) {
        gamer.stopTone();
        tetrisChirpPending = false;
    }

    // Background melody — simplified Korobeiniki (Tetris A-theme) using available frequency range
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
      // Serial.println("Game Over! Final Score: " + String(score));
        playLossTune();
        showScore(digitFrom(score, 2), digitFrom(score, 1)); // Display the final score
        delay(3000); // Wait for 3 seconds before resetting the game
        resetTetris();
        return; // Exit the loop so next call starts fresh
    }

    // Move the current piece down every second
    static unsigned long lastMoveTime = 0;
    if (millis() - lastMoveTime > moveInterval) {
      lastMoveTime = millis();
      // Serial.println("Attempting to move piece down...");
        if (canMove(currentX, currentY + 1)) {
            currentY++; // Move the piece down
            startLEDFlash(); // flash when piece moves down (gravity)
            lastMoveTime = millis();
            renderGridAndPiece();
        } else {
            // Place the piece on the grid
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (currentPiece[i][j] == 1) {
                        int x = currentX + j;
                        int y = currentY + i;
                        if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
                            grid[y][x] = 1; // Mark the grid cell as filled
                        }
                    }
                }
            }
            // Check for completed lines and update score
            // Serial.println("Piece placed. Checking for lines and creating new piece...");
            if (soundEnabled) { gamer.playTone(NOTE_C8); tetrisChirpPending = true; } // piece set-down thud
            checkLines();
            // Create a new piece
            createPiece();
            // Reset the position for the new piece
            currentX = 3;
            currentY = -3;
            // Check if the new piece can be placed, if not, game over
            if (!canMove(currentX, currentY) || !canMove(currentX, currentY + 2)) {
                gameOverT = true;
            }
            delay(100); // Short delay to prevent immediate input after placing a piece
            // Serial.println("New piece created. Current score: " + String(score) + ", Level: " + String(level) + ", Lines Cleared: " + String(linesCleared));
            renderGridAndPiece(); // Update the display with the current grid and piece
            currentY = -1;
        }
    }

    bool tetrisBtnPressed = false;
    if(gamer.isPressed(LEFT) && canMove(currentX - 1, currentY)) {
        currentX--; // Move left
        tetrisBtnPressed = true;
       renderGridAndPiece(); // Update the display with the current grid and piece
    } else if(gamer.isPressed(RIGHT) && canMove(currentX + 1, currentY)) {
        currentX++; // Move right
        tetrisBtnPressed = true;
        renderGridAndPiece(); // Update the display with the current grid and piece
    } else if(gamer.isPressed(DOWN) && canMove(currentX, currentY + 1) && !gamer.isHeld(DOWN)) {
        currentY++; // Move down faster
        tetrisBtnPressed = true;
        renderGridAndPiece(); // Update the display with the current grid and piece
    } else if(gamer.isHeld(DOWN){
        tetrisBtnPressed = true;
        while(canMove(currentX, currentY + 1)) currentY++; // hard drop to bottom
        renderGridAndPiece();
        while(gamer.isHeld(DOWN)) delay(10);
    } else if(gamer.isPressed(UP)) {
        rotatePiece(); // Rotate the piece
        tetrisBtnPressed = true;
        renderGridAndPiece(); // Update the display with the current grid and piece
    }
    if (tetrisBtnPressed) startLEDFlash(); // flash on any button press (move/rotate)
    if (soundEnabled && tetrisBtnPressed) { gamer.playTone(NOTE_D8); tetrisChirpPending = true; } // button press chirp
}

void checkLines() {
    for (int i = 0; i < gridHeight; i++) {
        bool lineComplete = true;
        for (int j = 0; j < gridWidth; j++) {
            if (grid[i][j] == 0) {
                lineComplete = false;
                break;
            }
        }
        if (lineComplete) {
            if (soundEnabled) { gamer.playTone(NOTE_A8); tetrisChirpPending = true; } // line clear reward chirp
            startLEDFlash(); // flash when a row is cleared
            currentX = 3;
            currentY = -1;
            // Clear the line
            for (int j = 0; j < gridWidth; j++) {
                grid[i][j] = 0; // Move down the lines above
                //animate
                renderGridAndPiece();
                delay(30);
            }
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < gridWidth; j++) {
                    grid[k][j] = grid[k - 1][j]; // Move down the lines above
                }
            }
            // Clear the top line
            for (int j = 0; j < gridWidth; j++) {
                grid[0][j] = 0;
            }
            linesCleared++;
            score += level; // Increase score based on level
            if (linesCleared % 7 == 0) { // Increase level every 10 lines
                level++;
                moveInterval = max(300, moveInterval - 200); // Decrease move interval to increase speed
            }
        }
    }
}

//define moveInterval as a global variable to control the speed of the pieces


void rotatePiece() {
    if (currentPieceType == O) return; // O piece is symmetric; rotation has no effect
    int temp[3][3] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            temp[j][2 - i] = currentPiece[i][j]; // Rotate clockwise
        }
    }
    // Check if the rotated piece can be placed in the current position
    if (canMove(currentX, currentY, temp)) {
        // If it can, copy the rotated piece back to currentPiece
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                currentPiece[i][j] = temp[i][j];
            }
        }
    }
}

// render grid[][] and currentPiece[][] at currentX, currentY to gamer.display[][]
void renderGridAndPiece() {
    // Render the grid
    for (int i = 0; i < gridHeight; i++) {
        for (int j = 0; j < gridWidth; j++) {
            if (grid[i][j] == 1) {
                gamer.display[j][i] = 1; // Set pixel for filled blocks
                // Serial.print('#');
            } else { // Set pixel for filled blocks
                // Serial.print('_');
                gamer.display[j][i] = 0;
              }
        }
        // Serial.println();
    }

    // Render the current piece
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (currentPiece[i][j] == 1) {
                int x = currentX + j;
                int y = currentY + i;
                if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
                    gamer.display[x][y] = 1; // Set pixel for current piece
                }
            }
        }
    }
    gamer.updateDisplay(); // Update the display after rendering
}

enum PieceType { I, O, T, S, Z, J, L };
PieceType currentPieceType;

//create 2x3 pieces
void createPiece() {
    // Clear the current piece
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            currentPiece[i][j] = 0;
        }
    }
    currentPieceType = (PieceType)random(0, 7); // Randomly select a piece type
    switch (currentPieceType) {
        case I:
            currentPiece[1][0] = 1;
            currentPiece[1][1] = 1;
            currentPiece[1][2] = 1;
            break;
        case O:
            currentPiece[0][0] = 1;
            currentPiece[0][1] = 1;
            currentPiece[1][0] = 1;
            currentPiece[1][1] = 1;
            break;
        case T:
            currentPiece[0][1] = 1;
            currentPiece[1][0] = 1;
            currentPiece[1][1] = 1;
            currentPiece[1][2] = 1;
            break;
        case S:
            currentPiece[0][1] = 1;
            currentPiece[0][2] = 1;
            currentPiece[1][0] = 1;
            currentPiece[1][1] = 1;
            break;
        case Z:
            currentPiece[0][0] = 1;
            currentPiece[0][1] = 1;
            currentPiece[1][1] = 1;
            currentPiece[1][2] = 1;
            break;
        case J:
            currentPiece[0][0] = 1;
            currentPiece[1][0] = 1;
            currentPiece[1][1] = 1;
            currentPiece[1][2] = 1;
            break;
        case L:
            currentPiece[0][2] = 1;
            currentPiece[1][0] = 1;
            currentPiece[1][1] = 1;
            currentPiece[1][2] = 1;
            break;
    }
    if (soundEnabled) { gamer.playTone(NOTE_G8); tetrisChirpPending = true; } // piece generation chirp
    startLEDFlash(); // flash when a new piece is created
}
//MARK:END OF GAME CODE

// ─────────────────────────────────────────────────────────────────────────────
//MARK: ALIEN (Space Invaders) setup animation
// Uses alien sprites from the original techwillsaveus/Gamer "Alien" example.
// ─────────────────────────────────────────────────────────────────────────────
void setupAlienAnim() {
  // Frame 0: alien1 sprite (body centred, legs down)
  alienAnim[0][0] = B00000000;
  alienAnim[0][1] = B00000000;
  alienAnim[0][2] = B01111110;
  alienAnim[0][3] = B01011010;
  alienAnim[0][4] = B01111110;
  alienAnim[0][5] = B00100100;
  alienAnim[0][6] = B00100100;
  alienAnim[0][7] = B01100110;
  // Frame 1: alien2 sprite (body jumped up one row, arms spread)
  alienAnim[1][0] = B00000000;
  alienAnim[1][1] = B01111110;
  alienAnim[1][2] = B01011010;
  alienAnim[1][3] = B01111110;
  alienAnim[1][4] = B00100100;
  alienAnim[1][5] = B01000010;
  alienAnim[1][6] = B11000011;
  alienAnim[1][7] = B00000000;
}

// ─────────────────────────────────────────────────────────────────────────────
//MARK: CONWAY setup animation
// Two frames of a glider from Conway's Game of Life.
// ─────────────────────────────────────────────────────────────────────────────
void setupConwayAnim() {
  // Frame 0: glider step 0  (.#. / ..# / ###)
  conwayAnim[0][0] = B01000000; // .#......
  conwayAnim[0][1] = B00100000; // ..#.....
  conwayAnim[0][2] = B11100000; // ###.....
  conwayAnim[0][3] = B00000000;
  conwayAnim[0][4] = B00000000;
  conwayAnim[0][5] = B00000000;
  conwayAnim[0][6] = B00000000;
  conwayAnim[0][7] = B00000000;
  // Frame 1: glider step 1  (#.# / .## / .#.)
  conwayAnim[1][0] = B10100000; // #.#.....
  conwayAnim[1][1] = B01100000; // .##.....
  conwayAnim[1][2] = B01000000; // .#......
  conwayAnim[1][3] = B00000000;
  conwayAnim[1][4] = B00000000;
  conwayAnim[1][5] = B00000000;
  conwayAnim[1][6] = B00000000;
  conwayAnim[1][7] = B00000000;
}

// ─────────────────────────────────────────────────────────────────────────────
//MARK: ALIEN (Space Invaders) GAME CODE
// Inspired by techwillsaveus/Gamer "Alien" example sprites.
// 3 rows × 3 cols of aliens march left; player at col 0 shoots right.
// UP/DOWN = move player  RIGHT = fire  START = exit
// ─────────────────────────────────────────────────────────────────────────────
// Alien state – bit-packed (bits 0-2 = cols 0-2 alive for that row)
byte sInvRows[3];    // y = 1, 3, 5 for rows 0, 1, 2
int8_t sInvBaseX;    // x of alien col 0 (cols at baseX, baseX+1, baseX+2)
int8_t sInvPlayerY;  // player row (0-7)
int8_t sInvBulletX;  // -1 = no bullet in flight
int8_t sInvBulletY;
byte sInvScore;
byte sInvMarchTick;

inline bool sInvAlive(byte row, byte col) { return (sInvRows[row] >> col) & 1; }
inline void sInvKill(byte row, byte col)  { sInvRows[row] &= ~(1 << col); }

byte sInvCount() {
  byte n = 0;
  for (byte r = 0; r < 3; r++)
    for (byte c = 0; c < 3; c++)
      if (sInvAlive(r, c)) n++;
  return n;
}

void resetAlienGame() {
  sInvRows[0] = 0x07; // 3 aliens
  sInvRows[1] = 0x07;
  sInvRows[2] = 0x07;
  sInvBaseX   = 5;    // alien cols at x=5,6,7
  sInvPlayerY = 3;    // player middle
  sInvBulletX = -1;   // no bullet
  sInvScore   = 0;
  sInvMarchTick = 0;
}

void alienLoop() {
  checkSoundToggle();
  updateLEDFlash();
  if (soundEnabled) gamer.stopTone(); // stop previous chirp

  // Clear display buffer
  for (int cx = 0; cx < 8; cx++)
    for (int cy = 0; cy < 8; cy++)
      gamer.display[cx][cy] = 0;

  // ── Bullet movement ──────────────────────────────────────────────────────
  if (sInvBulletX >= 0) {
    sInvBulletX++;
    if (sInvBulletX >= 8) {
      sInvBulletX = -1; // missed
    } else {
      // Collision with aliens
      for (byte r = 0; r < 3; r++) {
        byte ay = 1 + r * 2;
        if (sInvBulletY == ay) {
          for (byte c = 0; c < 3; c++) {
            if (sInvAlive(r, c) && (sInvBaseX + (int8_t)c) == sInvBulletX) {
              sInvKill(r, c);
              sInvBulletX = -1;
              sInvScore++;
              if (soundEnabled) gamer.playTone(NOTE_A8);
              startLEDFlash();
              break;
            }
          }
        }
        if (sInvBulletX < 0) break;
      }
    }
  }

  // ── Alien march ──────────────────────────────────────────────────────────
  byte marchRate = (sInvScore < 12) ? (20 - sInvScore) : 8;
  sInvMarchTick++;
  if (sInvMarchTick >= marchRate) {
    sInvMarchTick = 0;
    sInvBaseX--;
    if (sInvBaseX < 0) {
      // Aliens reached player column – game over
      for (byte b = 0; b < 4; b++) {
        for (int cx = 0; cx < 8; cx++) for (int cy = 0; cy < 8; cy++) gamer.display[cx][cy] = 0;
        gamer.updateDisplay(); delay(120);
        gamer.display[0][(byte)sInvPlayerY] = 1;
        gamer.updateDisplay(); delay(120);
      }
      playLossTune();
      showScore(sInvScore / 10, sInvScore % 10);
      delay(1500);
      resetAlienGame();
      return;
    }
  }

  // ── All aliens killed – next wave ─────────────────────────────────────
  if (sInvCount() == 0) {
    playWinTune();
    sInvRows[0] = sInvRows[1] = sInvRows[2] = 0x07;
    sInvBaseX = 5;
    sInvBulletX = -1;
  }

  // ── Player input ─────────────────────────────────────────────────────────
  if (gamer.isHeld(UP)   && sInvPlayerY > 0) sInvPlayerY--;
  if (gamer.isHeld(DOWN) && sInvPlayerY < 7) sInvPlayerY++;
  if (gamer.isPressed(RIGHT) && sInvBulletX < 0) {
    sInvBulletX = 1;
    sInvBulletY = sInvPlayerY;
    if (soundEnabled) gamer.playTone(NOTE_B7);
    startLEDFlash();
  }

  // ── Draw ─────────────────────────────────────────────────────────────────
  gamer.display[0][(byte)sInvPlayerY] = 1; // player
  if (sInvBulletX > 0)
    gamer.display[(byte)sInvBulletX][(byte)sInvBulletY] = 1;
  for (byte r = 0; r < 3; r++) {
    byte ay = 1 + r * 2;
    for (byte c = 0; c < 3; c++) {
      if (sInvAlive(r, c)) {
        int8_t ax = sInvBaseX + (int8_t)c;
        if (ax >= 0 && ax < 8) gamer.display[(byte)ax][ay] = 1;
      }
    }
  }
  gamer.updateDisplay();
  delay(80);
}

// ─────────────────────────────────────────────────────────────────────────────
//MARK: CONWAY'S GAME OF LIFE CODE
// From techwillsaveus/GamerCA2D (adapted to single-player launcher).
// Runs simulation automatically; RIGHT = reseed random pattern; START = exit.
// Uses bit-packed 8-byte rows (16 bytes total) for minimal SRAM use.
// ─────────────────────────────────────────────────────────────────────────────
#define CONWAY_STAGNATION_LIMIT 20  // reseed after this many unchanging generations

byte conwayCurr[8]; // each byte = one row; bit x (LSB=col 0) = cell at (x, row)
byte conwayNext[8];
byte conwayStuck;   // frames without any change (detect stasis/extinction)

void conwayRandomize() {
  for (byte i = 0; i < 8; i++) conwayCurr[i] = (byte)random(0, 256);
  conwayStuck = 0;
}

// Advance one generation; returns true if any cell changed.
bool conwayStep() {
  bool anyChange = false;
  for (byte y = 0; y < 8; y++) {
    conwayNext[y] = 0;
    for (byte x = 0; x < 8; x++) {
      byte alive = 0;
      for (int8_t dy = -1; dy <= 1; dy++)
        for (int8_t dx = -1; dx <= 1; dx++) {
          if (dx == 0 && dy == 0) continue;
          byte nx = (x + dx + 8) & 7;
          byte ny = (y + dy + 8) & 7;
          if ((conwayCurr[ny] >> nx) & 1) alive++;
        }
      bool curr = (conwayCurr[y] >> x) & 1;
      bool next = (alive == 3) || (curr && alive == 2);
      if (next) conwayNext[y] |= (1 << x);
      if (next != curr) anyChange = true;
    }
  }
  for (byte i = 0; i < 8; i++) conwayCurr[i] = conwayNext[i];
  return anyChange;
}

void resetConway() { conwayRandomize(); }

void conwayLoop() {
  checkSoundToggle();
  updateLEDFlash();

  bool changed = conwayStep();
  if (!changed) {
    conwayStuck++;
    if (conwayStuck > CONWAY_STAGNATION_LIMIT) conwayRandomize(); // static or extinct – reseed
  } else {
    conwayStuck = 0;
  }

  // RIGHT = plant new random seed manually
  if (gamer.isPressed(RIGHT)) conwayRandomize();

  // Render
  for (byte x = 0; x < 8; x++)
    for (byte y = 0; y < 8; y++)
      gamer.display[x][y] = (conwayCurr[y] >> x) & 1;
  gamer.updateDisplay();
  delay(180);
}

