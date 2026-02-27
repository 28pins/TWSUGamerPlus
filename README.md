# TWSUGamerPlus

![CI](https://github.com/28pins/TWSUGamerPlus/actions/workflows/ci.yml/badge.svg)
![Platform: AVR](https://img.shields.io/badge/platform-AVR%20(Uno)-green.svg)
![License: MIT (No AI)](https://img.shields.io/badge/license-MIT%20(No%20AI)-blue.svg)
![Version](https://img.shields.io/badge/version-3.0.0-informational.svg)

Arduino code and a lightweight hardware driver for the Technology Will Save Us DIY Gamer Kit. The repository bundles the `Gamer` library (LED matrix driver, button/IR/buzzer helpers) and a menu-driven sketch that runs fully featured games (not just demos), including added Tetris, richer light/sound effects, and memory optimizations on top of the base example.

## About this program

TWSUGamerPlus is an Arduino sketch and accompanying hardware-abstraction library built for the [Technology Will Save Us (TWSU) DIY Gamer Kit](https://www.techwillsaveus.com/shop/diy-kits/diy-gamer-kit-2/). Starting from the base TWSU example, this project adds:

- A **fully playable Tetris** implementation with piece rotation, soft-drop, and automatic speed progression.
- **Seven games** in a single sketch: Snake, Breakout, Simon Says, Flappy Bird, Tetris, Space Invaders, and Conway’s Game of Life.
- **Richer audio/visual feedback**: distinct win and loss tunes, non-blocking LED flashes, and per-game sound effects driven by a software tone engine.
- A **sound toggle** via the capacitive-touch pad (v1.9+ boards) so players can silence the buzzer without re-flashing.
- **Memory optimisations** that keep the whole program inside the ATmega328P’s 32 KB flash and 2 KB SRAM with room to spare.

## What’s inside
- `Gamer.h` / `Gamer.cpp`: the `Gamer` class that owns the 8x8 display buffer, scans buttons, drives the buzzer and IR LED, and exposes helpers like `printImage`, `printString`, `showScore`, and `playTone`.
- `TWSUGamerPlus-main.ino`: a single sketch with a launcher and seven games: Snake, Breakout, Simon, Flappy Bird, Tetris, Space Invaders, and Conway’s Game of Life.
- `library.properties`: Arduino metadata so the folder can live in `~/Arduino/libraries/Gamer`.

## Hardware

The DIY Gamer Kit is a soldering kit whose finished board is Arduino Uno compatible. Key components:

| Component | Detail |
|-----------|--------|
| Microcontroller | ATmega328P (Arduino Uno) |
| Display | 8×8 red LED matrix (64 individually addressable pixels) |
| Buttons | 5 tactile push-buttons: UP, DOWN, LEFT, RIGHT, START |
| Audio | Piezo buzzer driven by Timer2 PWM on pin 2 |
| Indicator LED | Onboard LED on pin 13 |
| Infrared | IR LED on pin 4 for 38 kHz IR transmission |
| Sensor | LDR (boards before v1.9) or capacitive-touch pad (v1.9+) on pin 5 |
| Power | USB or 9v battery (rechargeable recommended) via onboard regulator |

The `Gamer` library maps all hardware to named constants so sketches never use raw pin numbers directly.


## Getting started
1. Clone or download this repo. Place the folder in `~/Arduino/libraries/Gamer` (so the library files and the sketch sit together), or open the folder directly if you prefer to build from it.
2. Open `TWSUGamerPlus-main.ino` in the Arduino IDE.
3. Select **Board: Arduino Uno** and the correct serial port for your Gamer Kit.
4. Click **Upload**. No other libraries are required because the `Gamer` driver is included here.

## User guide

### Powering on
Connect the kit via USB (or insert batteries). The display shows a looping boot animation while the launcher starts up.

### Sound toggle
On v1.9+ hardware, tap the **capacitive-touch pad** on the PCB to toggle sound on and off. The toggle is edge-triggered (one touch = one state change). Sound starts off by default.

### Navigating the game menu
| Button | Action |
|--------|--------|
| `LEFT` / `RIGHT` | Cycle through the game icons |
| `START` | Launch the highlighted game |
| `START` (in-game) | Exit back to the launcher |

### Game controls

#### Snake
| Button | Action |
|--------|--------|
| `UP` / `DOWN` / `LEFT` / `RIGHT` | Steer the snake |

Eat the food pixel to grow. The game ends when the snake hits itself; your score is shown before returning to the menu.

#### Breakout
| Button | Action |
|--------|--------|
| `LEFT` / `RIGHT` | Move the paddle |

Keep the ball bouncing to break all the bricks. Miss the ball to lose a life. Score is shown on game over.

#### Simon Says
| Button | Action |
|--------|--------|
| `UP` / `DOWN` / `LEFT` / `RIGHT`¹ | Repeat the flashed sequence |

Watch the LED pattern carefully and repeat it exactly. The sequence grows by one step each round and the pace increases.

¹ The `RIGHT` direction is disabled by default (set `SIMON_RIGHT_ARROW_ENABLED` to `1` in the sketch to enable it).

#### Flappy Bird
| Button | Action |
|--------|--------|
| `UP` | Flap (rise) |

Guide the bird through the gaps between pipes. Colliding with a pipe or the ground ends the game and shows your score.

#### Tetris
| Button | Action |
|--------|--------|
| `LEFT` / `RIGHT` | Move piece horizontally |
| `DOWN` | Soft-drop (faster fall) |
| `UP` | Rotate piece clockwise |

Clear lines to score points. Speed increases every 10 cleared lines. The game ends when a new piece cannot be placed; score is displayed before returning to the menu.

#### Space Invaders
| Button | Action |
|--------|--------|
| `LEFT` / `RIGHT` | Move the cannon |
| `UP` | Fire |

Shoot the descending alien before it reaches the bottom. The alien speeds up each time it is hit.

#### Conway’s Game of Life
This is a zero-player simulation. Watch the cellular automaton evolve from a random seed. Press `START` to exit.


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
MIT (No AI version: see https://github.com/28pins/NoAiLicense?tab=License-1-ov-file). See `LICENSE` for details.

---

## Developer notes

### Repository structure

```
TWSUGamerPlus-main.ino   — slim main sketch (globals + helpers + includes)
Gamer.h / Gamer.cpp    — hardware-abstraction library
src/
  assets/
    progmem_assets.h   — all animation frames and image data in PROGMEM
  persistence/
    highscore.h        — header-only EEPROM high-score module (2-slot, CRC-8)
  games/
    game_interface.h   — GameDescriptor struct
    snake.h            — Snake game state + functions
    breakout.h         — Breakout game state + functions
    simon.h            — Simon Says game state + functions
    flappy.h           — Flappy Bird game state + functions
    tetris.h           — Tetris game state + functions
    alien.h            — Space Invaders game state + functions
    conway.h           — Conway's Game of Life state + functions
  launcher/
    launcher.h         — game registration, animation loop, START-to-launch
docs/
  memory-report.md     — before/after SRAM analysis
```

All game headers are `#include`d directly into the main `.ino` — they share one
translation unit, so there are no link-time issues and no need for separate
`.cpp` files.

### How to add a new game

1. Create `src/games/mygame.h` with include guards.
2. Declare any game-specific state variables (not `currentX`/`currentY`/`score`
   — those are shared globals in the main INO).
3. Implement `void resetMyGame()` and `void myGameLoop()`.
4. Add two PROGMEM animation frames to `src/assets/progmem_assets.h`:
   ```cpp
   static const byte myGame_pgm[2][8] PROGMEM = { { … }, { … } };
   ```
5. `#include "src/games/mygame.h"` in `TWSUGamerPlus-main.ino` (after the other
   game includes).
6. In `src/launcher/launcher.h`, add inside `launcherSetup()`:
   ```cpp
   registerGame("MYGAME", resetMyGame, myGameLoop, &myGame_pgm[0][0], 2);
   ```
7. Increase `LAUNCHER_MAX_GAMES` by 1.

### PROGMEM usage

Constant data (fonts, images, note arrays) is stored in flash via `PROGMEM` and
read at runtime with `pgm_read_byte()`. The helper `pgm_readimg(src, dst)` in
`progmem_assets.h` copies one 8-byte image row from PROGMEM to a stack buffer
for `gamer.printImage()`. String literals use the `F()` macro.

### High-score persistence

`src/persistence/highscore.h` provides a header-only EEPROM module:

- **Two-slot wear-levelling**: writes alternate between EEPROM addresses 0 and 8
  to spread erase cycles.
- **CRC-8 integrity**: each 8-byte slot ends with a CRC so corrupted or
  uninitialised EEPROM is detected cleanly.
- **Rate-limiting**: writes are suppressed if fewer than 1 s have elapsed since
  the last write.

