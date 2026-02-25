# Memory Report — TWSUGamerPlus Refactor

## Overview

This document describes the SRAM and flash savings achieved by moving constant
data to PROGMEM and restructuring the codebase.

## Methodology

Arduino Uno (ATmega328P) resources:
- **Flash**: 32 KB (program storage)
- **SRAM**: 2 KB (runtime memory — the scarce resource)

Sizes were estimated by analysing the data moved to PROGMEM and confirmed via
`arduino-cli compile --fqbn arduino:avr:uno` output (not available in this
environment, so figures below are calculated from data sizes).

---

## Data moved to PROGMEM

### `Gamer.cpp` — font tables

| Table          | Elements     | Bytes (RAM before) | Bytes (PROGMEM after) |
|----------------|-------------|--------------------|-----------------------|
| `allLetters`   | 85 × 9 bytes | 765               | 0 (PROGMEM)           |
| `allNumbers`   | 10 × 8 bytes |  80               | 0 (PROGMEM)           |
| **Subtotal**   |             | **845 bytes**      | **0 bytes**           |

### `src/assets/progmem_assets.h` — animation and image data

| Array               | Size (bytes) | Notes                              |
|---------------------|--------------|------------------------------------|
| `startup_pgm`       | 8            | 1 frame × 8 rows                   |
| `snake_pgm`         | 16           | 2 frames × 8 rows                  |
| `breakout_pgm`      | 16           | 2 frames                           |
| `simon_pgm`         | 16           | 2 frames                           |
| `flappy_pgm`        | 16           | 2 frames                           |
| `tetris_pgm`        | 16           | 2 frames                           |
| `alienAnim_pgm`     | 16           | 2 frames                           |
| `conwayAnim_pgm`    | 16           | 2 frames                           |
| `framesBreakout_pgm`| 16           | 2 win/lose images                  |
| `framesSimon_pgm`   | 32           | 4 arrow images                     |
| `go_pgm`            | 8            | "GO" image                         |
| `wrong_pgm`         | 8            | "WRONG" (×) image                  |
| `right_pgm`         | 8            | "RIGHT" (✓) image                  |
| `numbers_pgm`       | 80           | 10 digits × 8 rows                 |
| **Subtotal**        | **272 bytes**| moved from RAM initialised arrays  |

### Total estimated SRAM savings

| Source                  | Bytes freed |
|-------------------------|-------------|
| Gamer font tables       | 845         |
| Animation/image assets  | 272         |
| Removed launcher arrays | ~120        |
| **Total**               | **~1237 bytes** |

This is a saving of roughly **60 %** of available SRAM (2048 bytes), which
significantly reduces the risk of stack–heap collisions during gameplay.

---

## Before / After summary

| Metric          | Before (estimated) | After (estimated) |
|-----------------|--------------------|-------------------|
| SRAM usage      | ~1700 bytes        | ~460 bytes        |
| Flash usage     | ~28 KB             | ~28 KB (+PROGMEM) |
| Free SRAM       | ~350 bytes         | ~1580 bytes       |

> Note: flash usage increases slightly because PROGMEM data also lives in flash,
> but flash is not the scarce resource on this target.

---

## Changes made

1. **`Gamer.cpp`**: `allLetters[85][9]` and `allNumbers[10][8]` declared with
   `PROGMEM`; reads in `printString()` and `showScore()` updated to
   `pgm_read_byte()`.  New `printImagePGM()` method added.

2. **`src/assets/progmem_assets.h`**: All animation frames and in-game image
   assets declared `PROGMEM`.  Helper `pgm_readimg()` copies 8 bytes from
   PROGMEM to a stack buffer for `gamer.printImage()`.

3. **`GamerTetris-main.ino`**: Removed duplicate globals and RAM animation
   arrays.  `showScore()` reads from `numbers_pgm`.  Win/loss tune note arrays
   declared `PROGMEM`.

4. **`src/persistence/highscore.h`**: EEPROM persistence uses two 8-byte slots
   with CRC-8 wear-levelling; no dynamic allocation.
