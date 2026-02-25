# Contributing to TWSUGamerPlus

Thanks for your interest in improving TWSUGamerPlus!  
Please read this short guide before opening issues or submitting pull requests.

---

## Getting started

### Prerequisites

- A [TWSU DIY Gamer Kit](https://www.techwillsaveus.com/shop/diy-kits/diy-gamer-kit-2/) (v1.8 or v1.9).
- [arduino-cli](https://arduino.github.io/arduino-cli/latest/installation/) **or** the [Arduino IDE](https://www.arduino.cc/en/software) (v1.8+).
- `git`.

### Clone and build locally

```bash
git clone https://github.com/28pins/TWSUGamerPlus.git
cd TWSUGamerPlus

# Install the AVR toolchain (one-time setup)
arduino-cli core install arduino:avr

# Compile — this is the same check run by CI
arduino-cli compile --fqbn arduino:avr:uno .
```

### Flash to hardware

```bash
# Adjust --port to match your kit (e.g. /dev/ttyUSB0 or COM3)
arduino-cli upload --fqbn arduino:avr:uno --port /dev/ttyUSB0 .
```

---

## Reporting bugs

Use the [Bug report](.github/ISSUE_TEMPLATE/bug_report.md) issue template.  
Include:
- Gamer Kit hardware version (check the PCB silkscreen).
- Serial output (connect at 9600 baud — or set `#define GAMER_DEBUG 1` and recompile).
- Exact steps to reproduce.

## Requesting features

Use the [Feature request](.github/ISSUE_TEMPLATE/feature_request.md) issue template.  
Describe the impact on flash / SRAM (the ATmega328P has only 32 KB / 2 KB).

---

## Submitting pull requests

1. Fork the repo and create a feature branch from `main`.
2. Make your changes following the style guidelines below.
3. Verify the sketch still compiles: `arduino-cli compile --fqbn arduino:avr:uno .`
4. Flash to hardware and test the affected game(s) manually.
5. Open a PR using the [PR template](.github/PULL_REQUEST_TEMPLATE.md) and fill in all sections.

The CI workflow will automatically compile the sketch and report flash / SRAM usage.

---

## Adding a new game

See the **How to add a new game** section in [README.md](README.md#how-to-add-a-new-game).  
Short summary:
1. Create `src/games/mygame.h` with include guards.
2. Implement `resetMyGame()` and `myGameLoop()`.
3. Add two PROGMEM animation frames to `src/assets/progmem_assets.h`.
4. `#include "src/games/mygame.h"` in `TWSUGamerPlus-main.ino`.
5. Register the game in `src/launcher/launcher.h` and bump `LAUNCHER_MAX_GAMES`.

---

## Code style

- **Indentation**: tabs (match the surrounding files).
- **Types**: use `bool` not `boolean`; use `uint8_t`/`byte` for small integers.
- **Volatile**: only add `volatile` to variables that are genuinely shared between an ISR and `loop()` context. Game-state variables are never accessed from ISRs.
- **PROGMEM**: constant tables (images, melodies, font data) belong in PROGMEM and must be read with `pgm_read_byte()` or `pgm_readimg()`.
- **String literals**: use `F("…")` macro for Serial output; pass `const char*` to `printString()`.
- **No dynamic allocation**: avoid `new`, `malloc`, or `String` objects on AVR.

---

## License

By contributing you agree that your changes will be licensed under the repository's
[MIT (No AI) licence](LICENSE).
