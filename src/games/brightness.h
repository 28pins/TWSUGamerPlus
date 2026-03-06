// brightness.h — Brightness settings screen.
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

// Current brightness level (1–8); kept in sync with gamer.getBrightness()
static byte _brightSetting = 8;

// Draw a left-to-right bar of lit columns representing the current brightness.
// display[col][row] is the library's column-major convention (col = x, row = y).
// brightness 1 = column 0 only; brightness 8 = all eight columns.
static void drawBrightnessBar() {
	gamer.clear();
	byte bval = gamer.getBrightness();
	for (byte c = 0; c < bval; c++)
		for (byte r = 0; r < 8; r++)
			gamer.display[c][r] = 1;
	gamer.updateDisplay();
}

void resetBrightness() {
	_brightSetting = gamer.getBrightness();
	drawBrightnessBar();
}

void brightnessLoop() {
	bool changed = false;
	if (gamer.isPressed(UP) || gamer.isPressed(RIGHT)) {
		if (_brightSetting < 8) {
			_brightSetting++;
			gamer.setBrightness(_brightSetting);
			changed = true;
		}
	} else if (gamer.isPressed(DOWN) || gamer.isPressed(LEFT)) {
		if (_brightSetting > 1) {
			_brightSetting--;
			gamer.setBrightness(_brightSetting);
			changed = true;
		}
	}
	if (changed) drawBrightnessBar();
	delay(150);
}

#endif // BRIGHTNESS_H
