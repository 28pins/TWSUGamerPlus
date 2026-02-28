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
static byte   dinoObsType  = 0;     // 0 = cactus (ground), 1 = bird (air)
static int    dinoScore    = 0;
static bool   dinoOver     = false;
static unsigned long dinoLastTick = 0;
static unsigned int  dinoSpeed    = 280; // ms per game tick

void resetDino() {
	gamer.clear();
	dinoY        = 5;
	dinoVel      = 0;
	dinoJumping  = false;
	dinoDucking  = false;
	dinoObsX     = 9;
	dinoObsType  = 0;
	dinoScore    = 0;
	dinoOver     = false;
	dinoLastTick = millis();
	dinoSpeed    = 280;
}

void dinoLoop() {
	checkSoundToggle();
	updateLEDFlash();

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
		if (soundEnabled) gamer.playTone(NOTE_A8);
	}
	if (!dinoJumping) {
		dinoDucking = gamer.isHeld(DOWN);
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
			dinoObsX    = 8;
			dinoObsType = (random(3) == 0) ? 1 : 0; // 1/3 bird, 2/3 cactus
			dinoScore++;
			if (dinoScore > 99) dinoScore = 99;
			startLEDFlash();
			if (dinoSpeed > 140) dinoSpeed -= 5;
		}

		// Collision (obstacle passes through dino column 1)
		if (dinoObsX == 1) {
			bool hit = false;
			if (dinoObsType == 0) {
				// Cactus at rows 5-6: must jump above row 4
				if (dinoDucking || dinoY + 1 >= 5)
					hit = true;
			} else {
				// Bird at row 5: duck or jump above row 4 to avoid
				if (!dinoDucking && dinoY <= 5 && dinoY + 1 >= 5)
					hit = true;
			}
			if (hit) {
				dinoOver = true;
				playLossTune();
				return;
			}
		}
	}

	// ── Render ────────────────────────────────────────────────────────────
	gamer.clear();

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
		} else {
			gamer.display[dinoObsX][5] = 1;  // bird
		}
	}

	gamer.updateDisplay();
	delay(10);
}

#endif // DINO_H
