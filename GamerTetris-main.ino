#include <Gamer.h>

// Create a copy of the Gamer library.
Gamer gamer;

byte startup[16][8]; //declare at top of code
byte snake[16][8];  //snake animation
byte breakout[13][8]; //breakout anim
byte simon[20][8]; //simon anim
byte flappy[8][8]; //flappy anim
byte tetris[3][8]; //tetris anim
volatile byte animationLength[] = { //how long is each animation???
  16,13,20,8,3};
volatile byte animationFrame = 0; //what frame is it???
volatile byte gameNumber = 0; //what game is it???
volatile byte gameMax = 5; //how many games are there???

void setup() {
  gamer.begin();
  setupLogo(); //starting anim
  setupSnake(); //snake anim
  setupBreakout(); //breakout anim
  setupSimon(); //simon says anim
  setupFlappy(); //flappy anim
  setupScore(); //printString / showScore
  setupTetris(); //tetris anim
  setupImages(); //breakout win/lose images
  setupSimonImages(); //simon arrow and result images
  for(int i=0;i<16;i++) { //the animation
    gamer.printImage(startup[i]);
    delay(100);
  }
}

void loop() { //selector
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
      break;
    case 1:
      startBreakout(true);
      //breakout
      while(!gamer.isPressed(START)) {
        breakoutLoop();
      }
      break;
    case 2:
      resetSimon();
      while(!gamer.isPressed(START)) {
        simonLoop();
      }
      break;
    case 3:
      resetFlappy();
      while(!gamer.isPressed(START)) {
        flappyLoop();
      }
      break;
    case 4:
      resetTetris();
      while(!gamer.isPressed(START)) {
        tetrisLoop();
      }
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
    delay(100);
  }
}

void setupLogo() { //run this at the start
  startup[0][0] = B00000000;
  startup[0][1] = B00000000;
  startup[0][2] = B00000000;
  startup[0][3] = B00010000;
  startup[0][4] = B00001000;
  startup[0][5] = B00000000;
  startup[0][6] = B00000000;
  startup[0][7] = B00000000;
  startup[1][0] = B00000000;
  startup[1][1] = B00000000;
  startup[1][2] = B00000000;
  startup[1][3] = B00011000;
  startup[1][4] = B00011000;
  startup[1][5] = B00000000;
  startup[1][6] = B00000000;
  startup[1][7] = B00000000;
  startup[2][0] = B00000000;
  startup[2][1] = B00000000;
  startup[2][2] = B00110000;
  startup[2][3] = B00111000;
  startup[2][4] = B00011100;
  startup[2][5] = B00001100;
  startup[2][6] = B00000000;
  startup[2][7] = B00000000;
  startup[3][0] = B00000000;
  startup[3][1] = B00000000;
  startup[3][2] = B00111100;
  startup[3][3] = B00111100;
  startup[3][4] = B00111100;
  startup[3][5] = B00111100;
  startup[3][6] = B00000000;
  startup[3][7] = B00000000;
  startup[4][0] = B00000000;
  startup[4][1] = B01110000;
  startup[4][2] = B01111100;
  startup[4][3] = B01111100;
  startup[4][4] = B00111110;
  startup[4][5] = B00111110;
  startup[4][6] = B00001110;
  startup[4][7] = B00000000;
  startup[5][0] = B00000000;
  startup[5][1] = B01111110;
  startup[5][2] = B01111110;
  startup[5][3] = B01111110;
  startup[5][4] = B01111110;
  startup[5][5] = B01111110;
  startup[5][6] = B01111110;
  startup[5][7] = B00000000;
  startup[6][0] = B11110000;
  startup[6][1] = B11111110;
  startup[6][2] = B11111110;
  startup[6][3] = B11111110;
  startup[6][4] = B01111111;
  startup[6][5] = B01111111;
  startup[6][6] = B01111111;
  startup[6][7] = B00001111;
  startup[7][0] = B11111111;
  startup[7][1] = B11111111;
  startup[7][2] = B11111111;
  startup[7][3] = B11111111;
  startup[7][4] = B11111111;
  startup[7][5] = B11111111;
  startup[7][6] = B11111111;
  startup[7][7] = B11111111;
  startup[8][0] = B11111111;
  startup[8][1] = B11111111;
  startup[8][2] = B11111111;
  startup[8][3] = B11101111;
  startup[8][4] = B11110111;
  startup[8][5] = B11111111;
  startup[8][6] = B11111111;
  startup[8][7] = B11111111;
  startup[9][0] = B11111111;
  startup[9][1] = B11111111;
  startup[9][2] = B11111111;
  startup[9][3] = B11100111;
  startup[9][4] = B11100111;
  startup[9][5] = B11111111;
  startup[9][6] = B11111111;
  startup[9][7] = B11111111;
  startup[10][0] = B11111111;
  startup[10][1] = B11111111;
  startup[10][2] = B11001111;
  startup[10][3] = B11000111;
  startup[10][4] = B11100011;
  startup[10][5] = B11110011;
  startup[10][6] = B11111111;
  startup[10][7] = B11111111;
  startup[11][0] = B11111111;
  startup[11][1] = B11111111;
  startup[11][2] = B11000011;
  startup[11][3] = B11000011;
  startup[11][4] = B11000011;
  startup[11][5] = B11000011;
  startup[11][6] = B11111111;
  startup[11][7] = B11111111;
  startup[12][0] = B11111111;
  startup[12][1] = B10001111;
  startup[12][2] = B10000011;
  startup[12][3] = B10000011;
  startup[12][4] = B11000001;
  startup[12][5] = B11000001;
  startup[12][6] = B11110001;
  startup[12][7] = B11111111;
  startup[13][0] = B11111111;
  startup[13][1] = B10000001;
  startup[13][2] = B10000001;
  startup[13][3] = B10000001;
  startup[13][4] = B10000001;
  startup[13][5] = B10000001;
  startup[13][6] = B10000001;
  startup[13][7] = B11111111;
  startup[14][0] = B00000111;
  startup[14][1] = B00000001;
  startup[14][2] = B00000001;
  startup[14][3] = B00000000;
  startup[14][4] = B00000000;
  startup[14][5] = B10000000;
  startup[14][6] = B10000000;
  startup[14][7] = B11100000;
  startup[15][0] = B00000000;
  startup[15][1] = B00000000;
  startup[15][2] = B00000000;
  startup[15][3] = B00000000;
  startup[15][4] = B00000000;
  startup[15][5] = B00000000;
  startup[15][6] = B00000000;
  startup[15][7] = B00000000;
}

void setupSnake() { //run this at the start
  snake[0][0] = B00000000;
  snake[0][1] = B00000000;
  snake[0][2] = B00110100;
  snake[0][3] = B00000000;
  snake[0][4] = B00000000;
  snake[0][5] = B00000000;
  snake[0][6] = B00000000;
  snake[0][7] = B00000000;
  snake[1][0] = B00000000;
  snake[1][1] = B00000000;
  snake[1][2] = B00011100;
  snake[1][3] = B00000000;
  snake[1][4] = B00000000;
  snake[1][5] = B00000000;
  snake[1][6] = B00000000;
  snake[1][7] = B00000000;
  snake[2][0] = B00000000;
  snake[2][1] = B00000000;
  snake[2][2] = B00001100;
  snake[2][3] = B00000000;
  snake[2][4] = B00000000;
  snake[2][5] = B00000000;
  snake[2][6] = B00000000;
  snake[2][7] = B00000000;
  snake[3][0] = B00000000;
  snake[3][1] = B00000000;
  snake[3][2] = B00000100;
  snake[3][3] = B00000100;
  snake[3][4] = B00010000;
  snake[3][5] = B00000000;
  snake[3][6] = B00000000;
  snake[3][7] = B00000000;
  snake[4][0] = B00000000;
  snake[4][1] = B00000000;
  snake[4][2] = B00000000;
  snake[4][3] = B00000100;
  snake[4][4] = B00010100;
  snake[4][5] = B00000000;
  snake[4][6] = B00000000;
  snake[4][7] = B00000000;
  snake[5][0] = B00000000;
  snake[5][1] = B00000000;
  snake[5][2] = B00000000;
  snake[5][3] = B00000000;
  snake[5][4] = B00011100;
  snake[5][5] = B00000000;
  snake[5][6] = B00000000;
  snake[5][7] = B00000000;
  snake[6][0] = B00000000;
  snake[6][1] = B00000000;
  snake[6][2] = B00000000;
  snake[6][3] = B00000000;
  snake[6][4] = B00011000;
  snake[6][5] = B00000000;
  snake[6][6] = B00000000;
  snake[6][7] = B00000000;
  snake[7][0] = B00000000;
  snake[7][1] = B00000000;
  snake[7][2] = B00000000;
  snake[7][3] = B00000000;
  snake[7][4] = B00010000;
  snake[7][5] = B00010100;
  snake[7][6] = B00000000;
  snake[7][7] = B00000000;
  snake[8][0] = B00000000;
  snake[8][1] = B00000000;
  snake[8][2] = B00000000;
  snake[8][3] = B00000000;
  snake[8][4] = B00000000;
  snake[8][5] = B00011100;
  snake[8][6] = B00000000;
  snake[8][7] = B00000000;
  snake[9][0] = B00000000;
  snake[9][1] = B00000000;
  snake[9][2] = B00000000;
  snake[9][3] = B00000000;
  snake[9][4] = B00000000;
  snake[9][5] = B00001100;
  snake[9][6] = B00000000;
  snake[9][7] = B00000000;
  snake[10][0] = B00000000;
  snake[10][1] = B00000000;
  snake[10][2] = B00100000;
  snake[10][3] = B00000000;
  snake[10][4] = B00000100;
  snake[10][5] = B00000100;
  snake[10][6] = B00000000;
  snake[10][7] = B00000000;
  snake[11][0] = B00000000;
  snake[11][1] = B00000000;
  snake[11][2] = B00100000;
  snake[11][3] = B00000000;
  snake[11][4] = B00001100;
  snake[11][5] = B00000000;
  snake[11][6] = B00000000;
  snake[11][7] = B00000000;
  snake[12][0] = B00000000;
  snake[12][1] = B00000000;
  snake[12][2] = B00100000;
  snake[12][3] = B00000000;
  snake[12][4] = B00011000;
  snake[12][5] = B00000000;
  snake[12][6] = B00000000;
  snake[12][7] = B00000000;
  snake[13][0] = B00000000;
  snake[13][1] = B00000000;
  snake[13][2] = B00100000;
  snake[13][3] = B00000000;
  snake[13][4] = B00110000;
  snake[13][5] = B00000000;
  snake[13][6] = B00000000;
  snake[13][7] = B00000000;
  snake[14][0] = B00000000;
  snake[14][1] = B00000000;
  snake[14][2] = B00100000;
  snake[14][3] = B00100000;
  snake[14][4] = B00100000;
  snake[14][5] = B00000000;
  snake[14][6] = B00000000;
  snake[14][7] = B00000000;
  snake[15][0] = B00000000;
  snake[15][1] = B00000000;
  snake[15][2] = B00100000;
  snake[15][3] = B00100000;
  snake[15][4] = B00000000;
  snake[15][5] = B00000000;
  snake[15][6] = B00000000;
  snake[15][7] = B00000000;
}

void setupBreakout() { //run this at the start
  breakout[0][0] = B00000000;
  breakout[0][1] = B00000000;
  breakout[0][2] = B00111100;
  breakout[0][3] = B00000000;
  breakout[0][4] = B00100000;
  breakout[0][5] = B00110000;
  breakout[0][6] = B00000000;
  breakout[0][7] = B00000000;
  breakout[1][0] = B00000000;
  breakout[1][1] = B00000000;
  breakout[1][2] = B00111100;
  breakout[1][3] = B00010000;
  breakout[1][4] = B00000000;
  breakout[1][5] = B00011000;
  breakout[1][6] = B00000000;
  breakout[1][7] = B00000000;
  breakout[2][0] = B00000000;
  breakout[2][1] = B00000000;
  breakout[2][2] = B00101100;
  breakout[2][3] = B00000000;
  breakout[2][4] = B00001000;
  breakout[2][5] = B00001100;
  breakout[2][6] = B00000000;
  breakout[2][7] = B00000000;
  breakout[3][0] = B00000000;
  breakout[3][1] = B00000000;
  breakout[3][2] = B00101100;
  breakout[3][3] = B00000100;
  breakout[3][4] = B00000000;
  breakout[3][5] = B00011000;
  breakout[3][6] = B00000000;
  breakout[3][7] = B00000000;
  breakout[4][0] = B00000000;
  breakout[4][1] = B00000000;
  breakout[4][2] = B00101000;
  breakout[4][3] = B00000000;
  breakout[4][4] = B00001000;
  breakout[4][5] = B00001100;
  breakout[4][6] = B00000000;
  breakout[4][7] = B00000000;
  breakout[5][0] = B00000000;
  breakout[5][1] = B00000000;
  breakout[5][2] = B00101000;
  breakout[5][3] = B00010000;
  breakout[5][4] = B00000000;
  breakout[5][5] = B00011000;
  breakout[5][6] = B00000000;
  breakout[5][7] = B00000000;
  breakout[6][0] = B00000000;
  breakout[6][1] = B00000000;
  breakout[6][2] = B00001000;
  breakout[6][3] = B00000000;
  breakout[6][4] = B00001000;
  breakout[6][5] = B00001100;
  breakout[6][6] = B00000000;
  breakout[6][7] = B00000000;
  breakout[7][0] = B00000000;
  breakout[7][1] = B00000000;
  breakout[7][2] = B00001000;
  breakout[7][3] = B00000100;
  breakout[7][4] = B00000000;
  breakout[7][5] = B00011000;
  breakout[7][6] = B00000000;
  breakout[7][7] = B00000000;
  breakout[8][0] = B00000000;
  breakout[8][1] = B00000000;
  breakout[8][2] = B00000000;
  breakout[8][3] = B00000000;
  breakout[8][4] = B00001000;
  breakout[8][5] = B00011000;
  breakout[8][6] = B00000000;
  breakout[8][7] = B00000000;
  breakout[9][0] = B00000000;
  breakout[9][1] = B00000000;
  breakout[9][2] = B00000000;
  breakout[9][3] = B00010000;
  breakout[9][4] = B00000000;
  breakout[9][5] = B00110000;
  breakout[9][6] = B00000000;
  breakout[9][7] = B00000000;
  breakout[10][0] = B00000000;
  breakout[10][1] = B00000000;
  breakout[10][2] = B00100000;
  breakout[10][3] = B00000000;
  breakout[10][4] = B00000000;
  breakout[10][5] = B01100000;
  breakout[10][6] = B00000000;
  breakout[10][7] = B00000000;
  breakout[11][0] = B00000000;
  breakout[11][1] = B00000000;
  breakout[11][2] = B00000000;
  breakout[11][3] = B01000000;
  breakout[11][4] = B00000000;
  breakout[11][5] = B00110000;
  breakout[11][6] = B00000000;
  breakout[11][7] = B00000000;
  breakout[12][0] = B00000000;
  breakout[12][1] = B00000000;
  breakout[12][2] = B00000000;
  breakout[12][3] = B00000000;
  breakout[12][4] = B00100000;
  breakout[12][5] = B01100000;
  breakout[12][6] = B00000000;
  breakout[12][7] = B00000000;
}

void setupSimon() {
  simon[0][0] = B00000000; //up
  simon[0][1] = B00011000;
  simon[0][2] = B00111100;
  simon[0][3] = B01111110;
  simon[0][4] = B00011000;
  simon[0][5] = B00011000;
  simon[0][6] = B00011000;
  simon[0][7] = B00000000;
  simon[1][0] = B00000000; //blank
  simon[1][1] = B00000000;
  simon[1][2] = B00000000;
  simon[1][3] = B00000000;
  simon[1][4] = B00000000;
  simon[1][5] = B00000000;
  simon[1][6] = B00000000;
  simon[1][7] = B00000000;
  simon[2][0] = B00000000; //blank
  simon[2][1] = B00000000;
  simon[2][2] = B00000000;
  simon[2][3] = B00000000;
  simon[2][4] = B00000000;
  simon[2][5] = B00000000;
  simon[2][6] = B00000000;
  simon[2][7] = B00000000;
  simon[3][7] = B00000000; //down (aka up, but flipped)
  simon[3][6] = B00011000;
  simon[3][5] = B00111100;
  simon[3][4] = B01111110;
  simon[3][3] = B00011000;
  simon[3][2] = B00011000;
  simon[3][1] = B00011000;
  simon[3][0] = B00000000;
  simon[4][0] = B00000000; //blank
  simon[4][1] = B00000000;
  simon[4][2] = B00000000;
  simon[4][3] = B00000000;
  simon[4][4] = B00000000;
  simon[4][5] = B00000000;
  simon[4][6] = B00000000;
  simon[4][7] = B00000000;
  simon[5][0] = B00000000; //blank
  simon[5][1] = B00000000;
  simon[5][2] = B00000000;
  simon[5][3] = B00000000;
  simon[5][4] = B00000000;
  simon[5][5] = B00000000;
  simon[5][6] = B00000000;
  simon[5][7] = B00000000;
  simon[6][0] = B00000000; //blank
  simon[6][1] = B00000000;
  simon[6][2] = B00000000;
  simon[6][3] = B00000000;
  simon[6][4] = B00000000;
  simon[6][5] = B00000000;
  simon[6][6] = B00000000;
  simon[6][7] = B00000000;
  simon[7][0] = B00000000; //left
  simon[7][1] = B00010000;
  simon[7][2] = B00110000;
  simon[7][3] = B01111110;
  simon[7][4] = B01111110;
  simon[7][5] = B00110000;
  simon[7][6] = B00010000;
  simon[7][7] = B00000000;
  simon[8][0] = B00000000; //blank
  simon[8][1] = B00000000;
  simon[8][2] = B00000000;
  simon[8][3] = B00000000;
  simon[8][4] = B00000000;
  simon[8][5] = B00000000;
  simon[8][6] = B00000000;
  simon[8][7] = B00000000;
  simon[9][0] = B00000000; //blank
  simon[9][1] = B00000000;
  simon[9][2] = B00000000;
  simon[9][3] = B00000000;
  simon[9][4] = B00000000;
  simon[9][5] = B00000000;
  simon[9][6] = B00000000;
  simon[9][7] = B00000000;
  simon[10][0] = B00000000; //right (left but flipped)
  simon[10][1] = B00001000;
  simon[10][2] = B00001100;
  simon[10][3] = B01111110;
  simon[10][4] = B01111110;
  simon[10][5] = B00001100;
  simon[10][6] = B00001000;
  simon[10][7] = B00000000;
  simon[11][0] = B00000000; //blank
  simon[11][1] = B00000000;
  simon[11][2] = B00000000;
  simon[11][3] = B00000000;
  simon[11][4] = B00000000;
  simon[11][5] = B00000000;
  simon[11][6] = B00000000;
  simon[11][7] = B00000000;
  simon[12][0] = B00000000; //blank
  simon[12][1] = B00000000;
  simon[12][2] = B00000000;
  simon[12][3] = B00000000;
  simon[12][4] = B00000000;
  simon[12][5] = B00000000;
  simon[12][6] = B00000000;
  simon[12][7] = B00000000;
  simon[13][0] = B00000000; //up
  simon[13][1] = B00011000;
  simon[13][2] = B00111100;
  simon[13][3] = B01111110;
  simon[13][4] = B00011000;
  simon[13][5] = B00011000;
  simon[13][6] = B00011000;
  simon[13][7] = B00000000;
  simon[14][7] = B00000000; //down (aka up, but flipped)
  simon[14][6] = B00011000;
  simon[14][5] = B00111100;
  simon[14][4] = B01111110;
  simon[14][3] = B00011000;
  simon[14][2] = B00011000;
  simon[14][1] = B00011000;
  simon[14][0] = B00000000;
  simon[15][0] = B00000000; //right (left but flipped)
  simon[15][1] = B00001000;
  simon[15][2] = B00001100;
  simon[15][3] = B01111110;
  simon[15][4] = B01111110;
  simon[15][5] = B00001100;
  simon[15][6] = B00001000;
  simon[15][7] = B00000000;
  simon[16][0] = B00000000;
  simon[16][1] = B01100110;
  simon[16][2] = B00111100;
  simon[16][3] = B00011000;
  simon[16][4] = B00011000;
  simon[16][5] = B00111100;
  simon[16][6] = B01100110;
  simon[16][7] = B00000000;
  simon[17][0] = B00000000;
  simon[17][1] = B01100110;
  simon[17][2] = B00111100;
  simon[17][3] = B00011000;
  simon[17][4] = B00011000;
  simon[17][5] = B00111100;
  simon[17][6] = B01100110;
  simon[17][7] = B00000000;
  simon[18][0] = B00000000;
  simon[18][1] = B01100110;
  simon[18][2] = B00111100;
  simon[18][3] = B00011000;
  simon[18][4] = B00011000;
  simon[18][5] = B00111100;
  simon[18][6] = B01100110;
  simon[18][7] = B00000000;
  simon[19][0] = B00000000;
  simon[19][1] = B01100110;
  simon[19][2] = B00111100;
  simon[19][3] = B00011000;
  simon[19][4] = B00011000;
  simon[19][5] = B00111100;
  simon[19][6] = B01100110;
  simon[19][7] = B00000000;
}

void setupFlappy() { //run this at the start
  flappy[0][0] = B00000000;
  flappy[0][1] = B00000000;
  flappy[0][2] = B00100000;
  flappy[0][3] = B00000000;
  flappy[0][4] = B00000010;
  flappy[0][5] = B00000010;
  flappy[0][6] = B00000010;
  flappy[0][7] = B00000000;
  flappy[1][0] = B00000000;
  flappy[1][1] = B00100000;
  flappy[1][2] = B00000000;
  flappy[1][3] = B00000000;
  flappy[1][4] = B00000110;
  flappy[1][5] = B00000110;
  flappy[1][6] = B00000110;
  flappy[1][7] = B00000000;
  flappy[2][0] = B00000000;
  flappy[2][1] = B00000000;
  flappy[2][2] = B00100000;
  flappy[2][3] = B00000000;
  flappy[2][4] = B00001100;
  flappy[2][5] = B00001100;
  flappy[2][6] = B00001100;
  flappy[2][7] = B00000000;
  flappy[3][0] = B00000000;
  flappy[3][1] = B00000000;
  flappy[3][2] = B00000000;
  flappy[3][3] = B00100000;
  flappy[3][4] = B00011000;
  flappy[3][5] = B00011000;
  flappy[3][6] = B00011000;
  flappy[3][7] = B00000000;
  flappy[4][0] = B00000000;
  flappy[4][1] = B00000000;
  flappy[4][2] = B00100000;
  flappy[4][3] = B00000000;
  flappy[4][4] = B00110000;
  flappy[4][5] = B00110000;
  flappy[4][6] = B00110000;
  flappy[4][7] = B00000000;
  flappy[5][0] = B00000000;
  flappy[5][1] = B00100000;
  flappy[5][2] = B00000000;
  flappy[5][3] = B00000000;
  flappy[5][4] = B01100000;
  flappy[5][5] = B01100000;
  flappy[5][6] = B01100000;
  flappy[5][7] = B00000000;
  flappy[6][0] = B00000000;
  flappy[6][1] = B00000000;
  flappy[6][2] = B00100000;
  flappy[6][3] = B00000000;
  flappy[6][4] = B01000000;
  flappy[6][5] = B01000000;
  flappy[6][6] = B01000000;
  flappy[6][7] = B00000000;
  flappy[7][0] = B00000000;
  flappy[7][1] = B00000000;
  flappy[7][2] = B00000000;
  flappy[7][3] = B00100000;
  flappy[7][4] = B00000000;
  flappy[7][5] = B00000000;
  flappy[7][6] = B00000000;
  flappy[7][7] = B00000000;
}

void setupTetris() { //run this at the start
  tetris[0][0] = B00000000;
  tetris[0][1] = B00000000;
  tetris[0][2] = B00000000;
  tetris[0][3] = B00000000;
  tetris[0][4] = B00000000;
  tetris[0][5] = B00000000;
  tetris[0][6] = B00000000;
  tetris[0][7] = B00000000;
  tetris[1][0] = B00000000;
  tetris[1][1] = B00111100;
  tetris[1][2] = B00111100;
  tetris[1][3] = B00001100;
  tetris[1][4] = B00000000;
  tetris[1][5] = B00110000;
  tetris[1][6] = B00111100;
  tetris[1][7] = B00111100;
  tetris[2][0] = B00000000;
  tetris[2][1] = B00000000;
  tetris[2][2] = B00111100;
  tetris[2][3] = B00111100;
  tetris[2][4] = B00111100;
  tetris[2][5] = B00111100;
  tetris[2][6] = B00111100;
  tetris[2][7] = B00111100;
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
    startBreakout(false);
  }
  delay(50);
}
 
boolean outOfBounds(int xV, int yV) {
  if(xV > 8 || xV < 0) {
    return true;
  } 
  else if(yV > 8 || yV < 0) {
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


//ADVANCED code
volatile byte numbers[10][2][8];

void showScore(byte dig1,byte dig2) {
  byte result[8];
  for(int p=0;p<8;p++) {
    result[p]=numbers[dig1][0][p]^numbers[dig2][1][p];
  }
  gamer.printImage(result);
}

void setupScore() {
  numbers[1][0][0] = B10000000;
  numbers[1][0][1] = B10000000;
  numbers[1][0][2] = B10000000;
  numbers[1][0][3] = B10000000;
  numbers[1][0][4] = B10000000;
  numbers[1][0][5] = B10000000;
  numbers[1][0][6] = B10000000;
  numbers[1][0][7] = B10000000;

  numbers[1][1][0] = B00000100;
  numbers[1][1][1] = B00000100;
  numbers[1][1][2] = B00000100;
  numbers[1][1][3] = B00000100;
  numbers[1][1][4] = B00000100;
  numbers[1][1][5] = B00000100;
  numbers[1][1][6] = B00000100;
  numbers[1][1][7] = B00000100;

  numbers[2][0][0] = B11100000;
  numbers[2][0][1] = B00100000;
  numbers[2][0][2] = B00100000;
  numbers[2][0][3] = B11100000;
  numbers[2][0][4] = B10000000;
  numbers[2][0][5] = B10000000;
  numbers[2][0][6] = B10000000;
  numbers[2][0][7] = B11100000;

  numbers[2][1][0] = B00000111;
  numbers[2][1][1] = B00000001;
  numbers[2][1][2] = B00000001;
  numbers[2][1][3] = B00000111;
  numbers[2][1][4] = B00000100;
  numbers[2][1][5] = B00000100;
  numbers[2][1][6] = B00000100;
  numbers[2][1][7] = B00000111;

  numbers[3][0][0] = B11100000;
  numbers[3][0][1] = B00100000;
  numbers[3][0][2] = B00100000;
  numbers[3][0][3] = B01100000;
  numbers[3][0][4] = B00100000;
  numbers[3][0][5] = B00100000;
  numbers[3][0][6] = B00100000;
  numbers[3][0][7] = B11100000;

  numbers[3][1][0] = B00000111;
  numbers[3][1][1] = B00000001;
  numbers[3][1][2] = B00000001;
  numbers[3][1][3] = B00000011;
  numbers[3][1][4] = B00000001;
  numbers[3][1][5] = B00000001;
  numbers[3][1][6] = B00000001;
  numbers[3][1][7] = B00000111;

  numbers[4][0][0] = B10100000;
  numbers[4][0][1] = B10100000;
  numbers[4][0][2] = B10100000;
  numbers[4][0][3] = B11100000;
  numbers[4][0][4] = B00100000;
  numbers[4][0][5] = B00100000;
  numbers[4][0][6] = B00100000;
  numbers[4][0][7] = B00100000;

  numbers[4][1][0] = B00000101;
  numbers[4][1][1] = B00000101;
  numbers[4][1][2] = B00000101;
  numbers[4][1][3] = B00000111;
  numbers[4][1][4] = B00000001;
  numbers[4][1][5] = B00000001;
  numbers[4][1][6] = B00000001;
  numbers[4][1][7] = B00000001;

  numbers[5][0][0] = B11100000;
  numbers[5][0][1] = B10000000;
  numbers[5][0][2] = B10000000;
  numbers[5][0][3] = B11100000;
  numbers[5][0][4] = B00100000;
  numbers[5][0][5] = B00100000;
  numbers[5][0][6] = B00100000;
  numbers[5][0][7] = B11100000;

  numbers[5][1][0] = B00000111;
  numbers[5][1][1] = B00000100;
  numbers[5][1][2] = B00000100;
  numbers[5][1][3] = B00000111;
  numbers[5][1][4] = B00000001;
  numbers[5][1][5] = B00000001;
  numbers[5][1][6] = B00000001;
  numbers[5][1][7] = B00000111;

  numbers[6][0][0] = B11100000;
  numbers[6][0][1] = B10000000;
  numbers[6][0][2] = B10000000;
  numbers[6][0][3] = B11100000;
  numbers[6][0][4] = B10100000;
  numbers[6][0][5] = B10100000;
  numbers[6][0][6] = B10100000;
  numbers[6][0][7] = B11100000;

  numbers[6][1][0] = B00000111;
  numbers[6][1][1] = B00000100;
  numbers[6][1][2] = B00000100;
  numbers[6][1][3] = B00000111;
  numbers[6][1][4] = B00000101;
  numbers[6][1][5] = B00000101;
  numbers[6][1][6] = B00000101;
  numbers[6][1][7] = B00000111;

  numbers[7][0][0] = B11100000;
  numbers[7][0][1] = B00100000;
  numbers[7][0][2] = B00100000;
  numbers[7][0][3] = B00100000;
  numbers[7][0][4] = B00100000;
  numbers[7][0][5] = B00100000;
  numbers[7][0][6] = B00100000;
  numbers[7][0][7] = B00100000;

  numbers[7][1][0] = B00000111;
  numbers[7][1][1] = B00000001;
  numbers[7][1][2] = B00000001;
  numbers[7][1][3] = B00000001;
  numbers[7][1][4] = B00000001;
  numbers[7][1][5] = B00000001;
  numbers[7][1][6] = B00000001;
  numbers[7][1][7] = B00000001;

  numbers[8][0][0] = B11100000;
  numbers[8][0][1] = B10100000;
  numbers[8][0][2] = B10100000;
  numbers[8][0][3] = B11100000;
  numbers[8][0][4] = B10100000;
  numbers[8][0][5] = B10100000;
  numbers[8][0][6] = B10100000;
  numbers[8][0][7] = B11100000;

  numbers[8][1][0] = B00000111;
  numbers[8][1][1] = B00000101;
  numbers[8][1][2] = B00000101;
  numbers[8][1][3] = B00000111;
  numbers[8][1][4] = B00000101;
  numbers[8][1][5] = B00000101;
  numbers[8][1][6] = B00000101;
  numbers[8][1][7] = B00000111;

  numbers[9][0][0] = B11100000;
  numbers[9][0][1] = B10100000;
  numbers[9][0][2] = B10100000;
  numbers[9][0][3] = B11100000;
  numbers[9][0][4] = B00100000;
  numbers[9][0][5] = B00100000;
  numbers[9][0][6] = B00100000;
  numbers[9][0][7] = B11100000;

  numbers[9][1][0] = B00000111;
  numbers[9][1][1] = B00000101;
  numbers[9][1][2] = B00000101;
  numbers[9][1][3] = B00000111;
  numbers[9][1][4] = B00000001;
  numbers[9][1][5] = B00000001;
  numbers[9][1][6] = B00000001;
  numbers[9][1][7] = B00000111;

  numbers[0][0][0] = B11100000;
  numbers[0][0][1] = B10100000;
  numbers[0][0][2] = B10100000;
  numbers[0][0][3] = B10100000;
  numbers[0][0][4] = B10100000;
  numbers[0][0][5] = B10100000;
  numbers[0][0][6] = B10100000;
  numbers[0][0][7] = B11100000;

  numbers[0][1][0] = B00000111;
  numbers[0][1][1] = B00000101;
  numbers[0][1][2] = B00000101;
  numbers[0][1][3] = B00000101;
  numbers[0][1][4] = B00000101;
  numbers[0][1][5] = B00000101;
  numbers[0][1][6] = B00000101;
  numbers[0][1][7] = B00000111;
}

//FLAPPY code
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
        pipePos = 7; 
        pipeGap = 1 + rand()%4;
      }
           
      // Move the bird
      byte lastBirdPos = birdPos;
      if(gamer.isPressed(UP)) birdPos = max( birdPos - 1, 0 );//move the bird upwards when UP key is pressed
      else{
        birdPos++;//move the bird down
        if( birdPos >= 8 )//check if the bird hit the ground
        {
          gameOver = true;
          pipePos = lastPipePos;
          birdPos = lastBirdPos;
          ticks = 0;
        }
      }
      
      // Test for pipe collision
      if( (pipePos == 1 || pipePos == 0) && (birdPos < pipeGap || birdPos >= pipeGap + 3) )
      {
        gameOver = true;
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

//SIMON CODE
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
  sequence[x]=random(0,4);
  if(x>0) {
    for(byte p=3;p>0;p--) {
      showScore(0,p);
      delay(delayMils);
    }
    gamer.printImage(go); //move this later...
    delay(delayMils);
    for(int i=0;i<x;i++) {
      if(gamer.isHeld(START)) return;
      gamer.printImage(framesSimon[sequence[i]]);
      delay(delayMils);
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
        if(gamer.isPressed(UP)) key=0;
        if(gamer.isPressed(DOWN)) key=1;
        if(gamer.isPressed(LEFT)) key=2;
        if(gamer.isPressed(RIGHT)) key=3;
      }
      gamer.printImage(framesSimon[key]);
      //is it riiggghhhttt???
      if(key!=sequence[count]) {
        success = false; // game over...
        break;
      }
    }
    delayMils-=(delayMils/40);
    delay(delayMils);
    if(success) {
      x++; //they got it right, MAKE IT HARDER!
      gamer.printImage(right);
    } 
    else {
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


//SNAKE CODE
int currentX = 0;
int currentY = 0;
int dir = 1;
byte goalX = random(0,7);
byte goalY = random(0,7);
volatile byte snakeMap[8][8];
byte snakeLength = 2;
byte frames[11][8];
int score = 0;

void setupSnakeGame() {
  snakeLength = 2;
  score = 0;
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
  //gamer.clear, but DON'T UPDATE YET!!!!
  for(int x=0;x<8;x++) {
    for(int y=0;y<8;y++) {
      gamer.display[x][y] = LOW;
    }
  }
  //buttons should be here!
  //when upPressed etc. has been added, uncomment this next section and then comment out the random directions section:
  
  if(gamer.isPressed(UP)) dir=1;
  if(gamer.isPressed(RIGHT)) dir=2;
  if(gamer.isPressed(DOWN)) dir=3;
  if(gamer.isPressed(LEFT)) dir=4;
  
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

//TETRIS CODE
//Tetris Game for Arduino
unsigned long moveInterval = 2000; // Initial move interval in milliseconds
volatile int level = 1;
volatile int linesCleared = 0;
volatile bool gameOverT = false;
const int gridWidth = 8;
const int gridHeight = 8;
int grid[gridHeight][gridWidth] = {0}; // 0 for empty, 1 for filled
int currentPiece[3][3] = { //empty piece
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};

bool canMove(int x, int y, int piece[3][3] = currentPiece) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (piece[i][j] == 1) { // Check only filled blocks
                int newX = x + j;
                int newY = y + i;
                // Check boundaries
                if (newX < 0 || newX >= gridWidth || newY < 0 || newY >= gridHeight || grid[newY][newX] == 1) {
                    return false; // Can't move
                }

            }
        }
    }
    return true; // Can move
}

void resetTetris() {
    // Initialize the game state
    score = 0;
    level = 1;
    linesCleared = 0;
    gameOverT = false;
    currentX = 3;
    currentY = -3;
    
    // Clear the grid
    for (int i = 0; i < gridHeight; i++) {
        for (int j = 0; j < gridWidth; j++) {
        grid[i][j] = 0;
        }
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
    if (gameOverT) {
        showScore(digitFrom(score, 1), digitFrom(score, 2)); // Display the final score
        delay(5000); // Wait for 5 seconds before resetting the game
        return; // Exit the loop if the game is over
    }

    // Move the current piece down every second
    static unsigned long lastMoveTime = 0;
    if (millis() - lastMoveTime > moveInterval) {
        //flash led on pin 13
        digitalWrite(13, HIGH);
        if (canMove(currentX, currentY + 1)) {
            currentY++; // Move the piece down
            lastMoveTime = millis();
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
            checkLines();
            // Create a new piece
            createPiece();
            // Reset the position for the new piece
            currentX = 3;
            currentY = -3;
            // Check if the new piece can be placed, if not, game over
            if (!canMove(currentX, currentY)) {
                gameOverT = true;
            }
            delay(100); // Short delay to prevent immediate input after placing a piece
            digitalWrite(13, LOW);
        }
    }

    if(gamer.isPressed(LEFT) && canMove(currentX - 1, currentY)) {
        currentX--; // Move left
    } else if(gamer.isPressed(RIGHT) && canMove(currentX + 1, currentY)) {
        currentX++; // Move right
    } else if(gamer.isPressed(DOWN) && canMove(currentX, currentY + 1)) {
        currentY++; // Move down faster
    } else if(gamer.isPressed(UP)) {
        rotatePiece(); // Rotate the piece
    }
    gamer.printImage(0); // Clear the display before rendering
    renderGridAndPiece(); // Update the display with the current grid and piece
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
            // Clear the line
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
            if (linesCleared % 10 == 0) { // Increase level every 10 lines
                level++;
                moveInterval = max(300, moveInterval - 200); // Decrease move interval to increase speed
            }
        }
    }
}

//define moveInterval as a global variable to control the speed of the pieces


void rotatePiece() {
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
                gamer.display[j][i] = HIGH; // Set pixel for filled blocks
            }
        }
    }

    // Render the current piece
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (currentPiece[i][j] == 1) {
                int x = currentX + j;
                int y = currentY + i;
                if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
                    gamer.display[x][y] = HIGH; // Set pixel for current piece
                }
            }
        }
    }
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
            currentPiece[0][0] = 1;
            currentPiece[0][1] = 1;
            currentPiece[0][2] = 1;
            break;
        case O:
            currentPiece[0][0] = 1;
            currentPiece[0][1] = 1;
            currentPiece[1][0] = 1;
            currentPiece[1][1] = 1;
            break;
        case T:
            currentPiece[0][0] = 1;
            currentPiece[0][1] = 1;
            currentPiece[0][2] = 1;
            currentPiece[1][1] = 1;
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
}
