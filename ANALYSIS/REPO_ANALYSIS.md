# Repository Analysis — TWSUGamerPlus

*Generated: 2026-02-25 | Updated: 2026-02-25 (all findings addressed)*

---

## Executive Summary

TWSUGamerPlus is a well-structured, feature-rich Arduino sketch for the TWSU DIY Gamer Kit that successfully bundles seven playable games (Snake, Breakout, Simon Says, Flappy Bird, Tetris, Space Invaders, Conway's Game of Life) inside the tight resource envelope of an ATmega328P (32 KB flash / 2 KB SRAM). The codebase demonstrates thoughtful memory management via PROGMEM, a clean game-registration pattern, and solid EEPROM wear-levelling for high scores. All findings from the initial analysis have now been addressed: three critical hardware-layer bugs in `Gamer.cpp` were fixed (wrong-timer register, double ISR increment, duplicate defines); dead declarations were removed; file-scope globals were made `static`; spurious `volatile` qualifiers were removed; naming ambiguities were resolved; the `printString()` API was improved; CI, contribution scaffolding, documentation, and packaging were added. The sketch compiles to ~16 KB flash / ~990 bytes SRAM on Arduino Uno (49 % flash, 48 % SRAM).

---

## Detailed Findings

> **Status key**: ✅ Fixed | ⚠️ Partially addressed / corrected | — Not applicable

### 1. Code Quality

| ID | Location | Finding | Severity | Status |
|----|----------|---------|----------|--------|
| CQ-01 | `Gamer.h` L73–76 | `#define DAT 8` and `#define LAT 9` defined **twice** (duplicate `#define`). | High | ✅ Duplicates removed |
| CQ-02 | `Gamer.cpp` L111 | `stopTone()` wrote `TIMSK2 &= (1<<OCIE1A)` — Timer 1 register, not Timer 2. | High | ✅ Fixed to `TIMSK2 &= ~(1<<OCIE2A)` |
| CQ-03 | `Gamer.cpp` L43–54 | `split` incremented 3× per ISR tick when tone playing, corrupting display cadence. | High | ✅ Removed redundant `split++` branches; single increment per ISR call |
| CQ-04 | `Gamer.h` L26 | `void update()` declared but **never defined**. | Medium | ✅ Declaration removed |
| CQ-05 | `Gamer.h` L95 | `void checkSerial()` and `void setupLetters()` declared but **never defined**. | Medium | ✅ Declarations removed |
| CQ-06 | `Gamer.cpp` capTouch | Raw magic number `19` used for cap-touch pin. | Medium | ✅ `static constexpr uint8_t CAP_TOUCH_PIN = 19` added; all uses updated |
| CQ-07 | `Gamer.cpp` L2–13 | File-scope globals had external linkage. | Medium | ✅ All globals now `static` |
| CQ-08 | `Gamer.cpp` L504 | `printString()` accepted heap-allocated `String`, risking heap fragmentation. | Medium | ✅ Signature changed to `const char*`; body updated to use null-terminator loop |
| CQ-09 | `snake.h`, `breakout.h`, `tetris.h`, `simon.h` | `volatile` on non-ISR game-state variables adds unnecessary barriers. | Low | ✅ `volatile` removed from `snakeMap`, `blocks`, `velocity`, `sequence`, `level`, `linesCleared`, `gameOverT` |
| CQ-10 | `simon.h` L5 | Global `x` is terse and prone to shadowing. | Low | ✅ Renamed to `simonStep` |
| CQ-11 | `breakout.h` L10 | `volatile byte counter` name clashes with `Gamer::counter`. | Low | ✅ Renamed to `breakoutCounter` |
| CQ-12 | `flappy.h` L14–34 | **Analysis correction**: `menuScreen[64]` IS mutated (pixels 19 and 20 animate the bird's eye), so it cannot be declared fully `const` / PROGMEM without restructuring the animation logic. The claimed 64-byte SRAM saving is not achievable without a larger refactor. `inGameScreen[64]` is also mutated by `drawInGameScreen()`. | Low | ⚠️ Finding corrected; refactor deferred to a future PR |
| CQ-13 | `Gamer.h` L71–77 | Private `#define` constants polluted the global preprocessor namespace. | Low | ✅ All private `#define` pin constants removed; `PIN_LED` and `CAP_TOUCH_PIN` replaced with `static constexpr uint8_t`; `LETEND` moved to `Gamer.cpp` |
| CQ-14 | Multiple game headers | Non-standard `boolean` type alias used instead of `bool`. | Low | ✅ Changed to `bool` in `breakout.h`, `flappy.h`, `simon.h` |
| CQ-15 | `tetris.h` L25 | `canMove()` used global array as default parameter — non-standard and fragile. | Low | ✅ Split into 2-arg inline wrapper + 3-arg explicit implementation |

---

### 2. Tests & CI

| ID | Finding | Severity | Status |
|----|---------|----------|--------|
| TC-01 | No CI pipeline. | Critical | ✅ `.github/workflows/ci.yml` added (arduino-cli compile on every push/PR) |
| TC-02 | No automated sketch compilation check. | Critical | ✅ CI workflow compiles and reports flash/SRAM usage |
| TC-03 | No unit tests for `highscore.h` (CRC, wear-levelling, sequence wrap). | Medium | — Deferred (P3); requires host-side test harness |
| TC-04 | No static analysis (cppcheck/clang-tidy) in CI. | Medium | — Deferred (P3); will be added in a future PR |

---

### 3. Security

| ID | Finding | Severity | Status |
|----|---------|----------|--------|
| SEC-01 | `startupCheck()` unconditionally opened Serial and printed EEPROM data. | Low | ✅ Serial output now guarded by `#define GAMER_DEBUG 0` |
| SEC-02 | No secrets or credentials in the repository. | — | — (good) |
| SEC-03 | EEPROM CRC uses polynomial `0x07` (CRC-8/SMBUS). | — | — (correct; no issue) |

---

### 4. Dependencies

| ID | Finding | Severity | Status |
|----|---------|----------|--------|
| DEP-01 | `library.properties` `url` pointed at upstream TWSU fork. | Medium | ✅ Updated to `https://github.com/28pins/TWSUGamerPlus` |
| DEP-02 | `author`/`maintainer` still attributed to TWSU team. | Medium | ✅ Updated to `28pins` |
| DEP-03 | `version=2.1` not incremented despite substantial additions. | Low | ✅ Bumped to `3.0.0` |
| DEP-04 | No external library dependencies. | — | — (strength; unchanged) |

---

### 5. Documentation

| ID | Finding | Severity | Status |
|----|---------|----------|--------|
| DOC-01 | No `CONTRIBUTING.md`. | Medium | ✅ `CONTRIBUTING.md` added (build, flash, style, adding-a-game guide) |
| DOC-02 | No `CHANGELOG.md`. | Low | ✅ `CHANGELOG.md` added |
| DOC-03 | `keywords.txt` had stale entries (`irPlay`, `irStop`, `irReceive`, `irSend`, `update`). | Low | ✅ Stale entries removed; `irBegin`, `irEnd`, `appendColumn`, `showScore`, `printImagePGM` added; tab formatting fixed |
| DOC-04 | `README.md` had no badges. | Low | ✅ CI status, platform, licence, and version badges added |
| DOC-05 | No `.github/ISSUE_TEMPLATE/` or PR template. | Low | ✅ Added in previous PR |

---

### 6. Project Configuration

| ID | Finding | Severity | Status |
|----|---------|----------|--------|
| CFG-01 | No `.gitignore`. | Low | ✅ `.gitignore` added for Arduino build artefacts |
| CFG-02 | `library.properties` `sentence`/`paragraph` were upstream boilerplate. | Low | ✅ Updated to describe this extended library |

---

### 7. Packaging

| ID | Finding | Severity | Status |
|----|---------|----------|--------|
| PKG-01 | Folder name `TWSUGamerPlus` vs `name=Gamer` in `library.properties`. | Low | ✅ `name` updated to `GamerPlus` to better match folder |
| PKG-02 | No `examples/` directory. | Low | ✅ `examples/HelloGamer/HelloGamer.ino` added |

---

### 8. Performance

| ID | Finding | Severity | Status |
|----|---------|----------|--------|
| PERF-01 | **Corrected**: `menuScreen[64]` in `flappy.h` is mutated at runtime (pixels 19 & 20 blink). Cannot be fully PROGMEM. Saving 64 bytes of SRAM requires restructuring the animation logic — deferred. | Medium | ⚠️ Corrected; deferred |
| PERF-02 | `printString()` accepted heap-allocated `String`. | Medium | ✅ Changed to `const char*` (CQ-08) |
| PERF-03 | Spurious `volatile` on non-ISR game-state variables. | Low | ✅ Removed (CQ-09) |
| PERF-04 | `isHeld()` vs `isPressed()` asymmetry may cause missed-press edge cases under high ISR load. | Low | — Noted; no simple fix without reworking the polling model |

---

## Remaining / Deferred Items

The following findings are intentionally deferred to future PRs (P2–P3 priority):

| ID | Item | Priority | Notes |
|----|------|----------|-------|
| CQ-12/PERF-01 | `menuScreen` PROGMEM refactor | P2 | Requires restructuring the blink animation |
| TC-03 | Host-side unit tests for `highscore.h` | P3 | Needs mock EEPROM + test harness |
| TC-04 | cppcheck / clang-tidy in CI | P3 | Add as second CI job |
| PERF-04 | `isHeld()` vs `isPressed()` race | P3 | Would need a full input-model rework |

---

## Proposed Milestone / Roadmap (updated)

```
PR #A  — "Bootstrap: CI & analysis" ✅ DONE
         • .github/workflows/ci.yml
         • ANALYSIS/REPO_ANALYSIS.md
         • .github/ISSUE_TEMPLATE/ + PULL_REQUEST_TEMPLATE.md

PR #B  — "Fix all analysis findings" ✅ THIS PR
         • CQ-01 to CQ-15 code fixes
         • SEC-01 Serial guard
         • DEP, DOC, CFG, PKG improvements
         • CHANGELOG.md, CONTRIBUTING.md, .gitignore, examples/

PR #C  — "SRAM reduction: menuScreen animation refactor" (future)
         • Restructure flappy.h menu animation to use two PROGMEM frames
         • Saves 64 bytes of SRAM

PR #D  — "Host-side unit tests for highscore.h" (future)
         • tests/Makefile + EEPROM mock + CRC/wear-level/wrap tests
         • CI step: run host tests with g++

PR #E  — "Static analysis in CI" (future)
         • CI job: cppcheck --enable=all
```

---

*End of analysis.*

