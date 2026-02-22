//Tetris Game for Arduino

volatile int score = 0;
volatile int level = 1;
volatile int linesCleared = 0;
volatile bool gameOver = false;
const int gridWidth = 8;
const int gridHeight = 8;
int grid[gridHeight][gridWidth] = {0}; // 0 for empty, 1 for filled
int currentX = 3; // Starting X position of the piece
int currentY = 0; // Starting Y position of the piece
int currentPiece[3][3] = { //empty piece
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
}

void resetTetris() {
    // Initialize the game state
    score = 0;
    level = 1;
    linesCleared = 0;
    gameOver = false;
    currentX = 3;
    currentY = 0;
    
    // Clear the grid
    for (int i = 0; i < gridHeight; i++) {
        for (int j = 0; j < gridWidth; j++) {
        grid[i][j] = 0;
        }
    }

    createPiece();
}

void tetrisLoop() {
    if (gameOver) {
        showScore(); // Display the final score
        return; // Exit the loop if the game is over
    }

    // Move the current piece down every second
    static unsigned long lastMoveTime = 0;
    if (millis() - lastMoveTime > 1000) {
        movePieceDown();
        lastMoveTime = millis();
    }

    // Handle user input for moving left, right, or rotating the piece
    // (This part can be implemented based on your specific input method)
}

enum PieceType { I, O, T, S, Z, J, L };
PieceType currentPieceType;

//create 2x3 pieces
void createPiece() {
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
