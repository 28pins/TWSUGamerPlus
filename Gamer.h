// Gamer — hardware-abstraction library for the TWSU DIY Gamer Kit.
// Derived from the Technology Will Save Us (TWSU) Gamer library.
// Modifications © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

#ifndef Gamer_h
#define Gamer_h

#include "Arduino.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

class Gamer {
public:
	// Constructor
	Gamer();

	// Keywords
	#define UP 0
	#define LEFT 1
	#define RIGHT 2
	#define DOWN 3
	#define START 4
	//Note: Gamer v1.9 is Capacitive touch instead of LDR.
	#define LDR 5
	
	// Setup
	void begin();
	
	// Inputs
	bool isPressed(uint8_t input);
	bool isHeld(uint8_t input);
	//Note: Gamers preceding v1.9
	int ldrValue();
	void setldrThreshold(uint16_t threshold);
	//Note: Gamer v1.9
	bool capTouch();

	// Outputs
	void setRefreshRate(uint16_t refreshRate);
	void setBrightness(uint8_t level);      // 1 (dim) – 8 (full); default 8
	uint8_t getBrightness() const;
	void setLEDCompensation(bool enabled);  // enable/disable the +3 boost when LED is on
	bool getLEDCompensation() const;
	void updateDisplay();
	void allOn();
	void clear();
	void printImage(byte* img);
	void printImage(byte* img, int x, int y);
	void setLED(bool value);
	void toggleLED();
	void playTone(int note);
	void stopTone();
	void printString(const char* string);
	void appendColumn(byte* screen, byte col);
	void printImagePGM(const byte* pgm_img);

	// Infrared
	void irBegin();
	void irEnd();
	
	// Variables
	byte display[8][8];
	byte pulseCount;
	byte buzzerCount;
	byte nextRow;
	byte currentRow;
	byte counter;
	byte image[8];
	
	// Routines attached to the timer's ISR
	void isrRoutine();
	
private:
	
	// Pin assignments as class-scoped constants (avoids polluting the global macro namespace)
	static constexpr uint8_t PIN_LED       = 13;  // onboard indicator LED
	static constexpr uint8_t CAP_TOUCH_PIN = 19;  // capacitive-touch pad (v1.9+, A5)
	
	// Variables
	uint16_t _refreshRate;
	volatile uint8_t _brightness;  // effective level used by ISR (base + LED boost)
	uint8_t _baseBrightness;       // user-set level (1–8)
	bool _ledCompensation;         // whether to boost brightness when LED is on
	bool buttonFlags[6];
	unsigned long buttonLastPressed[6];
	int lastInputState[6];
	uint16_t ldrThreshold;
	
	// Functions
	void writeToDriver(byte dataOut);
	void writeToRegister(byte dataOut);
	void checkInputs();
	void updateRow();
	void updateEffectiveBrightness();  // recalc _brightness from _baseBrightness + LED state
	int currentInputState[6];
	bool tog;

	// Numbers and letters for printString (LETEND sentinel defined in Gamer.cpp)
	const static uint8_t allLetters[85][9] PROGMEM;
	const static uint8_t allNumbers[10][8] PROGMEM;
};

#endif
