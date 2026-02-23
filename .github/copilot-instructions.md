# Copilot Instructions for GamerTetris

## Project overview
This repository contains an Arduino C++ library (`Gamer`) and a multi-game sketch for the Technology Will Save Us DIY Gamer Kit (ATmega328P / Arduino Uno). The hardware has an 8×8 LED matrix, a piezo buzzer, five buttons (UP, DOWN, LEFT, RIGHT, START) and an IR LED. The project includes five fully featured games: Snake, Breakout, Simon, Flappy Bird, and Tetris.

## Repository layout
- `Gamer.h` / `Gamer.cpp` — the `Gamer` hardware-abstraction library (display, buttons, buzzer, IR).
- `GamerTetris-main.ino` — single sketch that contains the game launcher and all five games.
- `library.properties` — Arduino Library Manager metadata.
- `keywords.txt` — syntax-highlighting hints for the Arduino IDE.

## Tech stack and constraints
- **Language**: C++ (AVR-GCC dialect); Arduino framework.
- **Target**: ATmega328P (Arduino Uno). The code is AVR-only (`architectures=avr`). Do **not** use features or libraries that are incompatible with the AVR toolchain or that require more than ~2 KB SRAM / 32 KB flash.
- **No external libraries**: the `Gamer` driver is self-contained. Do not add third-party Arduino library dependencies.
- **Memory**: SRAM is scarce. Prefer `PROGMEM` / `pgm_read_byte` for large constant tables. Avoid dynamic allocation (`new`, `malloc`).
- **ISR safety**: the display and buzzer are driven from `TIMER2` ISRs. Keep ISR bodies short and do not call blocking functions from them. Variables shared with ISRs must be declared `volatile`.

## Building and uploading
```bash
# With arduino-cli (adjust port as needed):
arduino-cli compile --fqbn arduino:avr:uno .
arduino-cli upload  --fqbn arduino:avr:uno --port /dev/ttyUSB0 .
```
There is no automated test suite; verify changes by compiling and flashing to hardware.

## Code conventions
- Tabs for indentation (matches existing files).
- Keep game logic inside `GamerTetris-main.ino`; hardware-abstraction changes go in `Gamer.h` / `Gamer.cpp`.
- Button constants: `UP` (0), `LEFT` (1), `RIGHT` (2), `DOWN` (3), `START` (4), `LDR`/`capTouch` (5).
- Display: write to `gamer.display[row][col]` then call `gamer.updateDisplay()`.
- Scores are two-digit only; use `gamer.showScore(n)` which caps at 99.
- New games should follow the existing pattern: a standalone function called from the launcher loop, returning when the player presses START to exit.
