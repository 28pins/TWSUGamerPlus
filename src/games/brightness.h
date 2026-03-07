// brightness.h — Brightness settings screen.
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

// Synced from gamer on entry; written back on every change
static byte _brightSetting = 8;
static bool _ledCompSetting = true;

// Redraw the settings screen.
// Rows 0–6 (display[col][row] column-major): brightness bar — columns 0..(level-1) lit.
// Row 7: LED compensation indicator — all 8 pixels lit when comp ON, all dark when OFF.
static void drawBrightnessScreen() {
	gamer.clear();
	byte bval = gamer.getBrightness();
	// Brightness bar: top 7 rows
	for (byte c = 0; c < bval; c++)
		for (byte r = 0; r < 7; r++)
			gamer.display[c][r] = 1;
	// Comp indicator: bottom row
	if (gamer.getLEDCompensation())
		for (byte c = 0; c < 8; c++)
			gamer.display[c][7] = 1;
	gamer.updateDisplay();
}

void resetBrightness() {
	_brightSetting = gamer.getBrightness();
	_ledCompSetting = gamer.getLEDCompensation();
	drawBrightnessScreen();
}

void brightnessLoop() {
	bool changed = false;
	if (gamer.isPressed(UP) || gamer.isPressed(RIGHT)) {
		if (_brightSetting < 8) {
			_brightSetting++;
			gamer.setBrightness(_brightSetting);
			changed = true;
		}
	} else if (gamer.isPressed(DOWN)) {
		if (_brightSetting > 1) {
			_brightSetting--;
			gamer.setBrightness(_brightSetting);
			changed = true;
		}
	} else if (gamer.isPressed(LEFT)) {
		// Toggle LED compensation on/off
		_ledCompSetting = !_ledCompSetting;
		gamer.setLEDCompensation(_ledCompSetting);
		changed = true;
	}
	if (changed) drawBrightnessScreen();
	delay(150);
}

#endif // BRIGHTNESS_H
