//Tetris Game for Arduino

volatile int score = 0;
volatile int level = 1;
volatile int linesCleared = 0;
volatile bool gameOver = false;
const int gridWidth = 8;
const int gridHeight = 8;
int grid[gridHeight][gridWidth] = {0}; // 0 for empty, 1 for filled
int currentX = 3; // Starting X position of the piece
int currentY = -3; // Starting Y position of the piece
int currentPiece[3][3] = { //empty piece
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};

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

int digitFrom(int number, int position) {
    if (position < 1) return 0; // Invalid position
    if (number == 0) return 0; // Handle the case for 0
    if (number > 99) number = 99; // Cap the score at 99 for display purposes
    int digit = (number / (int)pow(10, position - 1)) % 10;
    return digit;
}

void tetrisLoop() {
    if (gameOver) {
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
                gameOver = true;
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
unsigned long moveInterval = 2000; // Initial move interval in milliseconds

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
