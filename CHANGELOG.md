# Changelog

All notable changes to TWSUGamerPlus are documented here.  
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

---

## [3.1.1] — 2026-03-03

### Bug Fixes
- **`flappy.h`**: fixed compile error — `rand(28, 40)` (stdlib `rand` takes no arguments) replaced with `random(28, 40)`; bare `rand()%4` replaced with `random(0, 4)`.
- **`snake.h`**: fixed food placement bug — `random(0,7)` changed to `random(0,8)` so food can now appear in column/row 7 (was blocking 12.5% of grid). Food respawn now retries until it lands on empty cell; initial placement avoids snake's spawn cell (0,0).
- **`tetris.h`**: fixed line-clearing score bug — `linesCleared += linesSinceLastDrop` inside loop was accumulating 1+2+3… instead of 1 per line; replaced with `linesCleared++` and removed `linesSinceLastDrop` variable.
- **`breakout.h`**: `saveHighScore()` now called unconditionally on game loss (was incorrectly skipped for scores < 10).
- **`TWSUGamerPlus.ino`**: EEPROM initialization guard changed from `== 0` to `!= 1` — fresh ATmega328P has all bytes at `0xFF`, so high scores incorrectly showed 255 on first boot. Initialization moved before `launcherSetup()`.

---

## [3.1.0] — 2026-03-03

### Improvements
- **`Gamer.cpp`**: removed unused variables `count` and `prevChar` (`-Wunused-variable` warnings); cast `ldrThreshold` to `(int)` in `checkInputs()` to fix `-Wsign-compare` warning; changed loop variables from `int x<=7` to `byte x<8` for consistency.
- **`alien.h`**: changed `long lastMove` to `unsigned long` and `int moveDelay` to `unsigned int` to fix `-Wsign-compare` warnings with `millis()` comparison.
- **`breakout.h`**: applied three changelog items from v3.0.0 that were marked done but never committed — removed 9 spurious `volatile` qualifiers, changed `boolean` to `bool`, renamed `counter` to `breakoutCounter`.
- **`snake.h`**: increased post-loss score display delay from 300 ms to 800 ms (was barely readable after loss tune).
- **`alien.h`**: added `checkSoundToggle()` and `updateLEDFlash()` calls inside the up-to-800 ms blocking `while` loop — sound toggle now works during full game tick.
- **`dino.h`**: replaced `gamer.clear()` (which internally calls `updateDisplay()`) with manual `display[][]` zeroing in render path — eliminated spurious all-black ISR frame between clear and render.

### Performance / Memory
- **`tetris.h`**: changed `int grid[8][8]` and `int currentPiece[3][3]` to `int8_t` (saves ~73 bytes SRAM); removed 9-line `digitFrom()` helper; changed `long lastDownPressTime` to `unsigned long`.
- **`flappy.h`**: changed `int ticks/birdPos/pipePos/pipeGap` to `unsigned int` or `int8_t` as appropriate.
- **`simon.h`**: changed `byte sequence[30]` to `byte sequence[SIMON_MAX_SEQUENCE]`; changed `int delayMils` to `uint16_t`; removed redundant sequence-zero loop in reset (values always written before read).
- **`breakout.h`** / **`alien.h`**: changed `int` loop variables to `byte` throughout counting loops; countdown loops use `int8_t`.
- **`TWSUGamerPlus.ino`**: changed `GAMER_DEBUG` from `1` to `0` (Serial was unconditionally open); changed `1024` to `EEPROM.length()`.
- **`tetris.h`**: removed duplicate `lastMoveTime = millis()` assignment in gravity branch; `max(300, …)` now uses consistent signedness to avoid implicit conversion.

---

## [3.0.0] — 2026-02-25

### Bug Fixes
- **`Gamer.cpp` `stopTone()`**: corrected `TIMSK2 &= (1<<OCIE1A)` typo to `TIMSK2 &= ~(1<<OCIE2A)` — was referencing Timer 1 instead of Timer 2, leaving the tone ISR enabled after `stopTone()`.
- **`Gamer.cpp` ISR `TIMER2_COMPA_vect`**: removed duplicate `split++` increments inside the tone-playing branches. `split` was incremented 3× per ISR tick when a tone was playing, corrupting the display-refresh cadence.
- **`Gamer.h`**: removed duplicate `#define DAT 8` and `#define LAT 9` entries.

### Improvements
- **`Gamer.h`**: removed undefined `update()`, `checkSerial()`, and `setupLetters()` declarations that would have caused linker errors if called.
- **`Gamer.h`**: replaced private `#define` pin constants with `static constexpr uint8_t` members (`PIN_LED`, `CAP_TOUCH_PIN`) to avoid polluting the global preprocessor namespace.
- **`Gamer.cpp`**: all file-scope globals are now `static`, preventing accidental external linkage.
- **`Gamer.cpp`**: `capTouch()` now uses `CAP_TOUCH_PIN` named constant (19 / A5) instead of magic number.
- **`Gamer.cpp`**: `printString()` signature changed from `String` (heap-allocated Arduino type) to `const char*` to prevent heap fragmentation on AVR.
- **Game headers**: removed spurious `volatile` qualifier from `snakeMap`, `blocks`, `velocity`, `sequence`, `level`, `linesCleared`, `gameOverT` — these variables are never accessed from an ISR.
- **`simon.h`**: renamed terse global `x` to `simonStep` for clarity.
- **`breakout.h`**: renamed `counter` to `breakoutCounter` to avoid name confusion with `Gamer::counter`.
- **`tetris.h`**: split `canMove()` into an explicit 3-argument version plus a 2-argument inline wrapper to replace the non-standard default-argument global-array pattern.
- **`flappy.h`**, **`breakout.h`**, **`simon.h`**: replaced non-standard `boolean` type alias with standard `bool`.
- **`TWSUGamerPlus-main.ino`**: `Serial` output in `startupCheck()` is now guarded by `#define GAMER_DEBUG 0`; set to `1` to re-enable diagnostic output.

### Metadata & Documentation
- **`library.properties`**: updated `name`, `version`, `author`, `maintainer`, `url`, `sentence`, and `paragraph` to reflect this fork.
- **`keywords.txt`**: removed stale entries (`update`, `irPlay`, `irStop`, `irReceive`, `irSend`); added `printImagePGM`, `irBegin`, `irEnd`, `appendColumn`, `showScore`; fixed tab formatting.
- **`README.md`**: added CI status badge, platform badge, licence badge, and version badge.
- Added `CONTRIBUTING.md` with build, flash, and coding-style instructions.
- Added `CHANGELOG.md` (this file).
- Added `.gitignore` for Arduino build artefacts.
- Added `examples/HelloGamer/HelloGamer.ino` minimal library example.
- Added `ANALYSIS/REPO_ANALYSIS.md` with full code-quality audit and improvement roadmap.
- Added `.github/workflows/ci.yml` (arduino-cli compile check on every push/PR).
- Added `.github/ISSUE_TEMPLATE/` and `.github/PULL_REQUEST_TEMPLATE.md`.

---

## [2.1.0] — 2025

*Upstream TWSU Gamer library version used as the starting point for this fork.*

### Added (relative to original TWSU example)
- Fully playable **Tetris** with piece rotation, soft-drop, and speed progression.
- **Space Invaders** game.
- **Conway's Game of Life** simulation.
- PROGMEM migration for font tables (`allLetters`, `allNumbers`), animation frames, and in-game image assets — saving ~1 237 bytes of SRAM.
- EEPROM high-score persistence with 2-slot wear-levelling and CRC-8 integrity check (`src/persistence/highscore.h`).
- Game-launcher with animated icons and sound toggle via capacitive-touch pad.
- `printImagePGM()` helper on `Gamer` class.
- Richer audio/visual feedback: win/loss tunes, non-blocking LED flash, per-game sound effects.
