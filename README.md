# GamerTetris

Arduino code and a lightweight hardware driver for the Technology Will Save Us DIY Gamer Kit. The repository bundles the `Gamer` library (LED matrix driver, button/IR/buzzer helpers) and a menu-driven sketch that lets you play several demo games on the 8x8 display.

## What’s inside
- `Gamer.h` / `Gamer.cpp`: the `Gamer` class that owns the 8x8 display buffer, scans buttons, drives the buzzer and IR LED, and exposes helpers like `printImage`, `printString`, `showScore`, and `playTone`.
- `GamerTetris-main.ino`: a single sketch with a launcher and five games: Snake, Breakout, Simon, Flappy, and Tetris.
- `library.properties`: Arduino metadata so the folder can live in `~/Arduino/libraries/Gamer`.

## Requirements
- DIY Gamer Kit (ATmega328P/Arduino Uno compatible, 8x8 LED matrix, buzzer, buttons).
- Arduino IDE (or arduino-cli) with the board set to Arduino Uno.

## Getting started
1. Clone or download this repo. Place the folder in `~/Arduino/libraries/Gamer` (so the library files and the sketch sit together), or open the folder directly if you prefer to build from it.
2. Open `GamerTetris-main.ino` in the Arduino IDE.
3. Select **Board: Arduino Uno** and the correct serial port for your Gamer Kit.
4. Click **Upload**. No other libraries are required because the `Gamer` driver is included here.

## Using the launcher
- On boot you’ll see looping icons. Press `LEFT`/`RIGHT` to pick a game and `START` to launch it. Press `START` again inside a game to return to the selector.
- Button constants available in code: `UP`, `DOWN`, `LEFT`, `RIGHT`, `START`, and `LDR` (or `capTouch` on newer hardware).

### Game controls
- **Snake**: `UP`/`RIGHT`/`DOWN`/`LEFT` steer the snake. Collect food to grow. Score shows when you collide with yourself.
- **Breakout**: `LEFT`/`RIGHT` move the paddle. Miss the ball to lose a life; the score shows on game over.
- **Simon**: Watch the sequence, then repeat with `UP`, `DOWN`, `LEFT`, `RIGHT`. Speed ramps up every round.
- **Flappy**: Press `UP` to flap through pipe gaps. Score shows after a crash.
- **Tetris**: `LEFT`/`RIGHT` move, `DOWN` soft-drops, `UP` rotates. Speed increases every 10 cleared lines; score displays when the board fills.

## Gamer library overview
- **Setup**: call `gamer.begin()` in `setup()` to configure pins, timers, and defaults.
- **Display**: write pixels into `gamer.display[8][8]` and call `gamer.updateDisplay()` to push them. Helpers: `printImage(byte* img)`, `printImage(img, x, y)`, `allOn()`, `clear()`, `appendColumn()`, `printString(String)`, `showScore(int)`, `setRefreshRate(uint16_t)`.
- **Inputs**: edge-triggered `isPressed(btn)` for single presses, `isHeld(btn)` for current state, `ldrValue()`/`setldrThreshold()` for light sensing on older boards, `capTouch()` for capacitive input on v1.9 hardware.
- **Buzzer**: `playTone(int note)` starts a tone, `stopTone()` stops it. The LED on pin 13 can be toggled with `setLED()`/`toggleLED()`.
- **Infrared**: `irBegin()` / `irEnd()` manage the 38 kHz carrier and share timer interrupts with the display refresh logic.

## Development tips
- The display is driven from a timer ISR; keep `loop()` work light to avoid jitter.
- `showScore` and in-sketch scoring helpers are two-digit only; values are capped for display.
- The library runs on AVR/Uno only (`architectures=avr`). Other boards will need pin and timer changes.

## License
MIT. See `LICENSE` for details.
