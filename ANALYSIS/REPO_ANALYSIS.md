# Repository Analysis — TWSUGamerPlus

*Generated: 2026-02-25 | Analyst: Copilot*

---

## Executive Summary

TWSUGamerPlus is a well-structured, feature-rich Arduino sketch for the TWSU DIY Gamer Kit that successfully bundles seven playable games (Snake, Breakout, Simon Says, Flappy Bird, Tetris, Space Invaders, Conway's Game of Life) inside the tight resource envelope of an ATmega328P (32 KB flash / 2 KB SRAM). The codebase demonstrates thoughtful memory management via PROGMEM, a clean game-registration pattern, and solid EEPROM wear-levelling for high scores. However, the repository lacks any automated CI/build validation, has no community-contribution scaffolding (issue/PR templates, CONTRIBUTING guide), contains several latent bugs in the hardware-abstraction layer (`Gamer.cpp`), misuses `volatile` on non-ISR-shared variables, retains stale metadata from the upstream fork, and stores large game-state arrays in SRAM that could be eliminated. Addressing the highest-priority items — CI, bug fixes, and the stale `library.properties` metadata — will raise confidence in the codebase and make it easier for contributors to participate safely.

---

## Detailed Findings

### 1. Code Quality

| ID | Location | Finding | Severity |
|----|----------|---------|----------|
| CQ-01 | `Gamer.h` L73–76 | `#define DAT 8` and `#define LAT 9` are each defined **twice** (duplicate `#define`). The second definitions silently win; first two are dead code. | High |
| CQ-02 | `Gamer.cpp` L111 | `stopTone()` writes `TIMSK2 &= (1<<OCIE1A)` — but `OCIE1A` belongs to **Timer 1**, not Timer 2. All other timer manipulation in this file uses Timer 2 registers. This is almost certainly a copy-paste typo; the intended mask is `(1<<OCIE2A)`. | High |
| CQ-03 | `Gamer.cpp` L43–44, 52–53 | Inside the `TIMER2_COMPA_vect` ISR the `split++` counter is incremented **twice** in several branches (once inside the `if/else` and once unconditionally at the end). This doubles the intended increment rate and corrupts the display-update cadence when a tone is playing. | High |
| CQ-04 | `Gamer.h` L26 | `void update()` is declared in the public API but **never defined** in `Gamer.cpp`. Any caller of `gamer.update()` will fail to link. The function body is also listed in `keywords.txt` L8. | Medium |
| CQ-05 | `Gamer.h` L95 | `void checkSerial()` is declared `private` but **never defined** in `Gamer.cpp`. Dead declaration clutters the API and will cause a linker error if ever called. | Medium |
| CQ-06 | `Gamer.cpp` L241–288 | `capTouch()` uses the **raw magic number `19`** (analog pin A5) instead of a named constant. The constant `RX` (pin 5) is defined in `Gamer.h` but refers to digital pin 5, not A5. A named `#define CAP_TOUCH_PIN 19` should be added and used consistently. | Medium |
| CQ-07 | `Gamer.cpp` L2–13 | File-scope globals (`count`, `toggleVal`, `split`, `ir`, `irTog`, `toneIsPlaying`, `playTog`, `toneStopped`, `prevChar`) are exposed at translation-unit scope. They should be `static` at minimum to prevent accidental external linkage. | Medium |
| CQ-08 | `Gamer.cpp` L504 | `printString()` accepts a `String` (heap-allocated Arduino String object). On AVR this causes heap fragmentation and can corrupt the stack. The signature should accept `const char*` or `const __FlashStringHelper*` instead. | Medium |
| CQ-09 | `src/games/snake.h` L9 | `volatile byte snakeMap[8][8]` is never accessed from an ISR; `volatile` adds unnecessary memory-barrier overhead on every access. Same issue in `breakout.h` (L6–14: `currentXBreakout`, `currentYBreakout`, `velocity`, `blocks`, etc.) and `tetris.h` (L7–9: `level`, `linesCleared`, `gameOverT`). | Low |
| CQ-10 | `src/games/simon.h` L5 | Global variable named `x` is extremely terse and clashes with the common loop variable name; it should be `simonLength` or `simonStep`. | Low |
| CQ-11 | `src/games/breakout.h` L10 | `volatile byte counter = 0` clashes in name (though not in scope) with the `public byte counter` member of the `Gamer` class. Rename to `breakoutCounter`. | Low |
| CQ-12 | `src/games/flappy.h` L14–34 | `inGameScreen[64]` and `menuScreen[64]` are 128 bytes of SRAM initialised to constants. They cannot easily go to PROGMEM as-is (they are mutated), but `menuScreen` is read-only and should be `const` / PROGMEM. | Low |
| CQ-13 | `Gamer.h` L71–77 | Private `#define` blocks inside a class definition pollute the **global preprocessor namespace**. Names like `CLK1`, `DAT`, `OE`, `LED`, `BUZZER`, `RX`, `TX`, `DEBOUNCETIME` are global macros visible everywhere, risking conflicts with user code. They should be `static constexpr uint8_t` members or moved to `Gamer.cpp`. | Low |
| CQ-14 | Multiple game headers | `boolean` type (Arduino alias) is used instead of the standard `bool`. Using `bool` is portable and unambiguous. | Low |
| CQ-15 | `src/games/tetris.h` L25 | `canMove()` has a default parameter `int piece[3][3] = currentPiece` — a non-const reference to a global. This is non-standard (default argument is a mutable global reference) and fragile; pass explicitly or make overloads. | Low |

---

### 2. Tests & CI

| ID | Finding | Severity |
|----|---------|----------|
| TC-01 | **No CI pipeline exists.** There are no `.github/workflows/` files. Every push to the repo is unvalidated; regressions can go unnoticed indefinitely. | Critical |
| TC-02 | No automated sketch compilation check. `arduino-cli compile --fqbn arduino:avr:uno` is the minimum viable validation and takes <2 min in CI. | Critical |
| TC-03 | No unit tests for the `highscore.h` module, which has non-trivial CRC, wear-levelling, and sequence-wrapping logic. A host-side test (CMake + googletest or Unity) would catch regressions. | Medium |
| TC-04 | No static analysis (cppcheck, clang-tidy) run in CI. Many of the CQ findings above would be surfaced automatically. | Medium |

---

### 3. Security

| ID | Finding | Severity |
|----|---------|----------|
| SEC-01 | `startupCheck()` in `TWSUGamerPlus-main.ino` unconditionally opens a Serial port at 9600 baud and prints EEPROM high-score data. On a deployed kit this leaks game state to anyone connected via USB. The Serial output should be guarded by a `#define DEBUG 0` compile-time flag. | Low |
| SEC-02 | No secrets or credentials in the repository (good). | — |
| SEC-03 | EEPROM CRC uses polynomial `0x07` (CRC-8/SMBUS) which is correct; no cryptographic concerns in this context. | — |

---

### 4. Dependencies

| ID | Finding | Severity |
|----|---------|----------|
| DEP-01 | `library.properties` `url` field still points to `http://github.com/techwillsaveus/Gamer` — the **upstream fork**, not this repository. The Arduino Library Manager will direct users to the wrong repo. | Medium |
| DEP-02 | `library.properties` `maintainer` and `author` fields still attribute the original TWSU team, not the 28pins maintainer. | Medium |
| DEP-03 | `library.properties` `version=2.1` has not been incremented despite substantial feature additions (seven games, PROGMEM refactor, EEPROM persistence). Semantic versioning discipline is absent. | Low |
| DEP-04 | No external library dependencies — the sketch is fully self-contained. This is a strength for the target platform. | — |

---

### 5. Documentation

| ID | Finding | Severity |
|----|---------|----------|
| DOC-01 | No `CONTRIBUTING.md` explaining how to submit issues, run builds locally, or add a new game. | Medium |
| DOC-02 | No `CHANGELOG.md` / `HISTORY.md` tracking changes between versions. | Low |
| DOC-03 | `keywords.txt` references `irPlay`, `irStop`, `irReceive`, `irSend` (L22–25) — these do not exist in `Gamer.h`/`Gamer.cpp`. Stale entries confuse IDE syntax highlighting. | Low |
| DOC-04 | `README.md` has no build-status badge, license badge, or version badge. Badges instantly communicate project health on GitHub. | Low |
| DOC-05 | No `.github/ISSUE_TEMPLATE/` or `.github/PULL_REQUEST_TEMPLATE.md` to guide contributors. | Low |

---

### 6. Project Configuration

| ID | Finding | Severity |
|----|---------|----------|
| CFG-01 | No `.gitignore` file. Arduino IDE and `arduino-cli` produce build artifacts (`build/`, `*.elf`, `*.hex`) that could accidentally be committed. | Low |
| CFG-02 | `library.properties` `sentence` and `paragraph` are still the original TWSU boilerplate and do not describe the extended game library. | Low |

---

### 7. Packaging

| ID | Finding | Severity |
|----|---------|----------|
| PKG-01 | Library folder name is `TWSUGamerPlus` but `library.properties` `name=Gamer`. Arduino Library Manager installs using the `name` field, so the folder name mismatch may cause confusion. | Low |
| PKG-02 | No example sketches in an `examples/` directory. Arduino Library Manager convention expects `examples/` for user-facing demos. | Low |

---

### 8. Performance

| ID | Finding | Severity |
|----|---------|----------|
| PERF-01 | `flappy.h`: `menuScreen[64]` is a read-only 64-byte SRAM array; it should be declared `static const byte PROGMEM` and read with `pgm_read_byte()`. Saves 64 bytes of precious SRAM. | Medium |
| PERF-02 | `Gamer::printString()` accepts `String` (heap-allocated). Each call allocates and frees ~N bytes on the heap, fragmenting the AVR heap which is tiny. The method should use `const char*` or `const __FlashStringHelper*`. | Medium |
| PERF-03 | `volatile` on `snakeMap`, `blocks[]`, `level`, `linesCleared`, `gameOverT`, `sequence[]`, `currentXBreakout`, etc. forces the compiler to emit a load/store for every access. These variables are only accessed from `loop()` context, never from the ISR. Removing `volatile` allows normal register-allocation optimisation. | Low |
| PERF-04 | `Gamer::isHeld()` reads `PINC` directly but `isPressed()` uses a flag array maintained by `checkInputs()`. The two polling paths are asymmetric; heavy use of `isHeld()` in game loops may cause missed-press edge cases if a button is released between ISR invocations. | Low |

---

## Prioritised Recommendations

> Labels: **P0** = do immediately, **P1** = next sprint, **P2** = backlog, **P3** = nice-to-have.
> Effort is wall-clock hours for an experienced C++/Arduino developer.
> ★ = top-5 immediate priorities.

| # | Priority | Title | Effort | Risk | Suggested PR Scope | Files to Modify |
|---|----------|-------|--------|------|--------------------|-----------------|
| R01 ★ | **P0** | Add CI workflow (arduino-cli compile) | 1 h | None | Single PR: `.github/workflows/ci.yml` | `.github/workflows/ci.yml` (new) |
| R02 ★ | **P0** | Fix duplicate `#define DAT`/`#define LAT` in `Gamer.h` | 0.25 h | Low | Bundle with next Gamer.h fix | `Gamer.h` L73–76 |
| R03 ★ | **P0** | Fix `TIMSK2 &= (1<<OCIE1A)` typo → `OCIE2A` in `stopTone()` | 0.25 h | Low | Bundle with R02 | `Gamer.cpp` L111 |
| R04 ★ | **P0** | Fix `split` double-increment inside ISR branches | 0.5 h | Low | Bundle with R02/R03 | `Gamer.cpp` L43–44, 52–53 |
| R05 ★ | **P0** | Fix stale `library.properties` URL, author, maintainer | 0.25 h | None | Single commit | `library.properties` |
| R06 | **P1** | Add named constant `CAP_TOUCH_PIN` and remove magic `19` | 0.25 h | Low | Part of Gamer.h clean-up PR | `Gamer.h`, `Gamer.cpp` |
| R07 | **P1** | Remove undefined `update()` and `checkSerial()` declarations | 0.25 h | Low | Part of Gamer.h clean-up PR | `Gamer.h` |
| R08 | **P1** | Mark `Gamer.cpp` file-scope globals `static` | 0.5 h | Low | Part of Gamer.cpp clean-up PR | `Gamer.cpp` L1–13 |
| R09 | **P1** | Add `.github/ISSUE_TEMPLATE/` and `PULL_REQUEST_TEMPLATE.md` | 1 h | None | Single PR: community scaffolding | `.github/` (new files) |
| R10 | **P1** | Add `CONTRIBUTING.md` | 1 h | None | Bundle with R09 | `CONTRIBUTING.md` (new) |
| R11 | **P1** | Add `.gitignore` for Arduino build artefacts | 0.25 h | None | Single commit | `.gitignore` (new) |
| R12 | **P1** | Remove incorrect `volatile` from non-ISR game-state variables | 1 h | Low | PR per game file or one sweep | `snake.h`, `breakout.h`, `tetris.h`, `simon.h` |
| R13 | **P1** | Move `flappy.h` `menuScreen[64]` to PROGMEM | 0.5 h | Low | Part of SRAM-reduction PR | `src/games/flappy.h` |
| R14 | **P2** | Replace `String` with `const char*` in `printString()` | 2 h | Medium | Separate PR: API change | `Gamer.h`, `Gamer.cpp` |
| R15 | **P2** | Guard `Serial` output in `startupCheck()` behind `#define DEBUG` | 0.5 h | Low | Single commit | `TWSUGamerPlus-main.ino` |
| R16 | **P2** | Rename `x` → `simonLength` in `simon.h` | 0.25 h | Low | Part of rename/cleanup PR | `src/games/simon.h` |
| R17 | **P2** | Rename `counter` → `breakoutCounter` in `breakout.h` | 0.25 h | Low | Part of rename/cleanup PR | `src/games/breakout.h` |
| R18 | **P2** | Move private `#define` constants in `Gamer.h` to `static constexpr` | 2 h | Medium | Separate PR: header clean-up | `Gamer.h`, `Gamer.cpp` |
| R19 | **P2** | Clean up stale `keywords.txt` entries (`irPlay`, `irStop`, etc.) | 0.25 h | None | Single commit | `keywords.txt` |
| R20 | **P2** | Add `CHANGELOG.md` with initial history | 0.5 h | None | Documentation PR | `CHANGELOG.md` (new) |
| R21 | **P2** | Update `library.properties` description fields | 0.25 h | None | Bundle with R05 | `library.properties` |
| R22 | **P3** | Add host-side unit tests for `highscore.h` logic | 4 h | Low | Separate PR: test infrastructure | `tests/` (new directory) |
| R23 | **P3** | Add `examples/` directory with a minimal blink sketch | 1 h | None | Separate PR: packaging | `examples/` (new) |
| R24 | **P3** | Fix `canMove()` default-parameter anti-pattern in `tetris.h` | 1 h | Medium | Tetris refactor PR | `src/games/tetris.h` |
| R25 | **P3** | Replace `boolean` with `bool` throughout | 0.5 h | Low | Sweep PR | Multiple game headers |

---

## Concrete Change Outlines

### R01 — Add CI workflow (P0 ★)

**File to create:** `.github/workflows/ci.yml`

```yaml
name: CI
on: [push, pull_request]
jobs:
  compile:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: arduino/setup-arduino-cli@v2
      - run: |
          arduino-cli core install arduino:avr
          arduino-cli compile --fqbn arduino:avr:uno .
```

**Tests/checks required:** The workflow itself is the check — a non-zero exit code fails the PR.

---

### R02–R04 — Fix Gamer.cpp/Gamer.h hardware bugs (P0 ★)

**Files:** `Gamer.h`, `Gamer.cpp`

- `Gamer.h`: Delete the second `#define DAT 8` (L74) and second `#define LAT 9` (L75).
- `Gamer.cpp` L111: Change `TIMSK2 &= (1<<OCIE1A)` → `TIMSK2 &= ~(1<<OCIE2A)`.
- `Gamer.cpp`: In `TIMER2_COMPA_vect` ISR, remove the redundant unconditional `split++` at the bottom of each `if/else` branch so `split` is incremented exactly once per branch.

**Tests/checks required:** CI compile check passes; manual flash + verify buzzer/display work correctly after tone stop.

---

### R05 — Fix `library.properties` metadata (P0 ★)

**File:** `library.properties`

```
name=GamerPlus
version=3.0.0
author=28pins
maintainer=28pins <github.com/28pins>
sentence=Seven-game sketch and hardware driver for the TWSU DIY Gamer Kit.
paragraph=Adds Tetris, Space Invaders, Conway's Game of Life, EEPROM high scores, and PROGMEM optimisations to the base TWSU Gamer library.
url=https://github.com/28pins/TWSUGamerPlus
architectures=avr
```

**Tests/checks required:** None (metadata only); confirm Arduino Library Manager URL resolves.

---

### R09–R10 — Community scaffolding (P1)

**Files to create:**
- `.github/ISSUE_TEMPLATE/bug_report.md`
- `.github/ISSUE_TEMPLATE/feature_request.md`
- `.github/PULL_REQUEST_TEMPLATE.md`
- `CONTRIBUTING.md`

Include: hardware setup instructions, how to compile with `arduino-cli`, how to run the CI check locally, coding style guidance, how to add a new game (already documented in README — link to it).

---

### R13 — `menuScreen` PROGMEM (P1)

**File:** `src/games/flappy.h`

Declare `menuScreen` as `static const byte PROGMEM` and update `flappyLoop()` to copy it to a stack buffer via `pgm_readimg()` before use.

```cpp
// Before
byte menuScreen[] = { 0,0,0,1,1, … };

// After
static const byte menuScreen_pgm[] PROGMEM = { 0,0,0,1,1, … };
// In flappyLoop(), when menu == true:
byte buf[8]; pgm_readimg(menuScreen_pgm + row*8, buf);
```

**Saves:** 64 bytes of SRAM.

---

### R22 — Host-side unit tests for `highscore.h` (P3)

**Directory:** `tests/`

Use a stub `EEPROM` class and `millis()` mock to exercise:
- Fresh EEPROM → `loadHighScores()` returns 0.
- CRC corruption detected → fallback to zeros.
- Wear-level alternation: odd calls go to slot 1, even to slot 0.
- Sequence-number wrap-around (0xFF → 0x00) handled correctly.
- Rate-limiting: second write within 1 s is suppressed.

Build system: a small `Makefile` with `g++ -std=c++17` targeting the host.

---

## Proposed Milestone / Roadmap

The goal is small, independently reviewable PRs that land safely without disrupting the main development line.

```
PR #A  — "Bootstrap: CI & analysis" (this PR)
         • .github/workflows/ci.yml
         • ANALYSIS/REPO_ANALYSIS.md
         • .github/ISSUE_TEMPLATE/ + PULL_REQUEST_TEMPLATE.md

PR #B  — "Fix hardware-layer bugs" (R02, R03, R04, R06, R07, R08)
         • Gamer.h: remove duplicate defines, add CAP_TOUCH_PIN,
           remove dead update()/checkSerial() declarations
         • Gamer.cpp: fix OCIE1A typo, fix split double-increment,
           make file-scope globals static

PR #C  — "Fix library metadata & docs" (R05, R19, R21, R20, CFG-01)
         • library.properties: URL, author, version, description
         • keywords.txt: remove stale irPlay/irStop/irReceive/irSend
         • CHANGELOG.md, .gitignore

PR #D  — "Remove spurious volatile & rename globals" (R09, R12, R16, R17)
         • snake.h, breakout.h, tetris.h, simon.h: strip volatile
         • simon.h: rename x → simonLength
         • breakout.h: rename counter → breakoutCounter
         • CONTRIBUTING.md

PR #E  — "SRAM reductions" (R13, R14, PERF-01)
         • flappy.h: menuScreen → PROGMEM
         • Gamer: printString(const char*) overload

PR #F  — "Misc quality & packaging" (R15, R18, R23, R24, R25)
         • DEBUG guard on Serial output
         • private #defines → static constexpr
         • examples/ directory
         • canMove() default-param fix
         • boolean → bool sweep

PR #G  — "Host-side unit tests" (R22, TC-03, TC-04)
         • tests/ with EEPROM mock and highscore_test.cpp
         • CI step: run host tests with g++
         • Optional: cppcheck step in CI
```

Each PR above is scoped to touch ≤ 4 files and can be reviewed independently. Start with PR #A (this PR) to establish CI before making any source changes, so all subsequent PRs are automatically validated.

---

*End of analysis.*
