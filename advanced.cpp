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
