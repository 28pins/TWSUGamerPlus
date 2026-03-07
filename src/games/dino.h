// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

#ifndef DINO_H
#define DINO_H

// ── Dino Runner game state ────────────────────────────────────────────────────
static int8_t dinoY        = 5;     // top row of dino sprite (standing = 5)
static int8_t dinoVel      = 0;     // jump velocity (negative = upward)
static bool   dinoJumping  = false;
static bool   dinoDucking  = false;
static int8_t dinoObsX     = 9;     // obstacle column (scrolls right → left)
static byte   dinoObsType  = 0;     // 0 = cactus (ground), 1 = bird (air), 2 = giant bird, 3 = wide cactus (2x2), 4 = flying bar (3x1)
static int    dinoScore    = 0;
static bool   dinoOver     = false;
static unsigned long dinoLastTick = 0;
static unsigned int  dinoSpeed    = 160; // ms per game tick

void resetDino() {
	gamer.clear();
	dinoY        = 5;
	dinoVel      = 0;
	dinoJumping  = false;
	dinoDucking  = false;
	dinoObsX     = 9;
	dinoObsType  = 0;
	if (startFromHighScore && startingHighScore > 0) {
		// Start with half the high score
		dinoScore = startingHighScore / 2;
		// Speed increases by 5ms per obstacle passed, minimum 60ms
		dinoSpeed = max(60, 160 - dinoScore * 5);
	} else {
		dinoScore = 0;
		dinoSpeed = 160;
	}
	dinoOver     = false;
	dinoLastTick = millis();
}

void dinoLoop() {
	checkSoundToggle();
	updateLEDFlash();
	if (soundEnabled) gamer.stopTone();

	// ── Game-over: show score, then auto-restart ──────────────────────────
	if (dinoOver) {
		showScore(dinoScore / 10, dinoScore % 10);
		saveHighScore(dinoScore, 7);
		delay(2000);
		resetDino();
		return;
	}

	// ── Input ─────────────────────────────────────────────────────────────
	if (gamer.isPressed(UP) && !dinoJumping) {
		dinoJumping = true;
		dinoDucking = false;
		dinoVel     = -2;
		startLEDFlash();
		if (soundEnabled) gamer.playTone(NOTE_A8);
	}
	// DOWN pressed/held: cancel any active jump and duck
	if (gamer.isHeld(DOWN)) {
		if (dinoJumping) {
			dinoY       = 5;
			dinoVel     = 0;
			dinoJumping = false;
		}
		if (!dinoDucking) startLEDFlash();
		dinoDucking = true;
	} else if (!dinoJumping) {
		dinoDucking = false;
	}

	// ── Tick-gated update ─────────────────────────────────────────────────
	unsigned long now = millis();
	if (now - dinoLastTick >= dinoSpeed) {
		dinoLastTick = now;

		// Jump physics
		if (dinoJumping) {
			dinoY += dinoVel;
			dinoVel++;
			if (dinoY >= 5) {
				dinoY       = 5;
				dinoVel     = 0;
				dinoJumping = false;
			}
		}

		// Advance obstacle
		dinoObsX--;
		if (dinoObsX < 0) {
			dinoObsX    = random(8, 11); // 13-18 ticks off-screen (5-10 column gap)
			// Random obstacle type selection
			byte typeRoll = random(5);
			if (typeRoll == 0) {
				dinoObsType = 1; // bird (air)
			} else if (typeRoll == 1) {
				dinoObsType = 2; // giant bird
			} else if (typeRoll == 2) {
				dinoObsType = 3; // wide cactus (2x2)
			} else if (typeRoll == 3) {
				dinoObsType = 4; // flying bar (1x3)
			} else {
				dinoObsType = 0; // cactus (ground)
			}
			dinoScore++;
			if (dinoScore > 99) dinoScore = 99;
			startLEDFlash();
			if (soundEnabled) gamer.playTone(NOTE_E8);
			if (dinoSpeed > 60) dinoSpeed -= 4;
		}

		// Collision (obstacle passes through dino column 1)
		if (dinoObsX == 1) {
			bool hit = false;
			if (dinoObsType == 0) {
				// Cactus at rows 5-6: must jump above row 4
				if (dinoDucking || dinoY + 1 >= 5)
					hit = true;
			} else if (dinoObsType == 2) {
				// Giant bird at rows 3-5: must duck (can't jump over)
				if(!dinoDucking || dinoJumping)
					hit = true;
			} else if (dinoObsType == 1) {
				// Bird at row 5: duck or jump above row 4 to avoid
				if (!dinoDucking && dinoY <= 5 && dinoY + 1 >= 5)
					hit = true;
			} else if (dinoObsType == 3) {
				// Wide cactus (2x2) at rows 5-6, columns X and X+1: must jump above row 4
				// Check collision at columns 1 and 2
				if (dinoDucking || dinoY + 1 >= 5)
					hit = true;
			} else if (dinoObsType == 4) {
				// Flying bar (3x1 horizontal) at row 5: duck or jump above row 4 to avoid
				if (!dinoDucking && dinoY <= 5 && dinoY + 1 >= 5)
					hit = true;
			}
			if (hit) {
				dinoOver = true;
				playLossTune();
				return;
			}
		}

		// Additional collision check for wide cactus (type 3) when its second column overlaps dino column 1
		if (dinoObsX == 0 && dinoObsType == 3) {
			bool hit = false;
			if (dinoDucking || dinoY + 1 >= 5)
				hit = true;
			if (hit) {
				dinoOver = true;
				playLossTune();
				return;
			}
		}

		// Additional collision checks for horizontal flying bar (type 4) as it spans columns 1, 0, and -1
		if ((dinoObsX == 1 || dinoObsX == 0 || dinoObsX == -1) && dinoObsType == 4) {
			bool hit = false;
			if (!dinoDucking && dinoY <= 5 && dinoY + 1 >= 5)
				hit = true;
			if (hit) {
				dinoOver = true;
				playLossTune();
				return;
			}
		}
	}

	// ── Render ────────────────────────────────────────────────────────────
	// Zero display array directly (avoids gamer.clear()'s extra updateDisplay call)
	for (byte i = 0; i < 8; i++)
		for (byte j = 0; j < 8; j++)
			gamer.display[i][j] = 0;

	// Ground (row 7)
	for (byte i = 0; i < 8; i++) gamer.display[i][7] = 1;

	// Dino
	if (dinoDucking) {
		gamer.display[1][6] = 1;           // crouched – single pixel
	} else {
		if (dinoY >= 0 && dinoY < 8)
			gamer.display[1][dinoY] = 1;     // top pixel
		if (dinoY + 1 >= 0 && dinoY + 1 < 8)
			gamer.display[1][dinoY + 1] = 1; // bottom pixel
	}

	// Obstacle
	if (dinoObsX >= 0 && dinoObsX < 8) {
		if (dinoObsType == 0) {
			gamer.display[dinoObsX][5] = 1;  // cactus top
			gamer.display[dinoObsX][6] = 1;  // cactus bottom
		} else if (dinoObsType == 1) {
			gamer.display[dinoObsX][5] = 1;  // bird
		} else if (dinoObsType == 2) {
			// Giant bird (1x3 vertical)
			gamer.display[dinoObsX][5] = 1;
			gamer.display[dinoObsX][4] = 1;
			gamer.display[dinoObsX][3] = 1;
		} else if (dinoObsType == 3) {
			// Wide cactus (2x2) - ground obstacle
			gamer.display[dinoObsX][5] = 1;
			gamer.display[dinoObsX][6] = 1;
			if (dinoObsX + 1 < 8) {
				gamer.display[dinoObsX + 1][5] = 1;
				gamer.display[dinoObsX + 1][6] = 1;
			}
		} else if (dinoObsType == 4) {
			// Flying bar (3x1 horizontal) - flying obstacle spanning 3 columns
			gamer.display[dinoObsX][5] = 1;
			if (dinoObsX + 1 < 8) {
				gamer.display[dinoObsX + 1][5] = 1;
			}
			if (dinoObsX + 2 < 8) {
				gamer.display[dinoObsX + 2][5] = 1;
			}
		}
	}
	// Handle second column of wide cactus when it's at X = -1
	if (dinoObsX == -1 && dinoObsType == 3) {
		gamer.display[0][5] = 1;
		gamer.display[0][6] = 1;
	}
	// Handle remaining columns of horizontal flying bar when partially off-screen
	if (dinoObsX == -1 && dinoObsType == 4) {
		gamer.display[0][5] = 1;
		gamer.display[1][5] = 1;
	}
	if (dinoObsX == -2 && dinoObsType == 4) {
		gamer.display[0][5] = 1;
	}

	gamer.updateDisplay();
	delay(10);
}

#endif // DINO_H
