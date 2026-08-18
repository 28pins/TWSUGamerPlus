# GamerEmulator — iOS SwiftUI Emulator

A complete iOS/iPadOS SwiftUI emulator of the **TWSU DIY Gamer Kit** hardware and all nine built-in games from [TWSUGamerPlus](https://github.com/28pins/TWSUGamerPlus).

---

## Features

| Feature | Details |
|---------|---------|
| **Display** | 8 × 8 LED matrix rendered as amber rounded-rect pixels with 8-level brightness |
| **Input** | D-pad (▲ ▼ ◀ ▶) + START button; cap-touch pad emulated as SOUND toggle |
| **Sound** | Sine-wave tone generator via AVAudioEngine; frequency derived from hardware OCR2A values |
| **Persistence** | High scores stored in UserDefaults (one slot per game, 0–255) |
| **Games** | Snake · Breakout · Simon · Flappy Bird · Tetris · Alien · Conway · Dino · Brightness |
| **Launcher** | Left/Right to browse, UP for high-score mode, START to launch, EXIT button in-game |

---

## Requirements

| Requirement | Minimum |
|-------------|---------|
| iOS / iPadOS | 16.0 |
| Xcode | 15.0 |
| Swift | 5.9 |

---

## Building

1. Open **`GamerEmulator.xcodeproj`** in Xcode.
2. Select your device or a simulator.
3. Press **⌘R** to build and run.

No third-party dependencies — the project uses only Apple's standard frameworks
(`SwiftUI`, `AVFoundation`, `Foundation`).

---

## Project structure

```
GamerEmulator/
├── GamerEmulatorApp.swift        – @main entry point
├── ContentView.swift             – Root SwiftUI view
├── Info.plist
├── Assets.xcassets/
├── Hardware/
│   ├── GamerHardware.swift       – Emulated Gamer hardware (display, input, LED, sound)
│   └── SoundEngine.swift         – AVAudioEngine sine-wave generator
├── GameAssets/
│   └── GameAssets.swift          – All bitmaps & melody data (ported from PROGMEM)
├── Persistence/
│   └── HighScoreStore.swift      – UserDefaults high-score storage
├── Games/
│   ├── GameProtocol.swift        – Game protocol
│   ├── SnakeGame.swift
│   ├── TetrisGame.swift
│   ├── BreakoutGame.swift
│   ├── SimonGame.swift
│   ├── FlappyGame.swift
│   ├── DinoGame.swift
│   ├── AlienGame.swift
│   ├── ConwayGame.swift
│   └── BrightnessGame.swift
├── Launcher/
│   └── Launcher.swift            – ObservableObject managing the game menu
└── Views/
    ├── MatrixDisplayView.swift   – 8×8 LED grid
    ├── ButtonsView.swift         – D-pad, START, cap-touch buttons
    └── GamerView.swift           – Root emulator screen
```

---

## Architecture

### Game loop (async/await)
Each game implements the `Game` protocol with two methods:
- `reset(fromHighScore:startingScore:)` – initialise state
- `loop(gamer:) async` – one game-tick; may `await gamer.delay(N)` for animations

The `Launcher` runs the active game inside a `Task { @MainActor }`, which lets
`await Task.sleep()` suspend the main actor between frames — freeing SwiftUI to
re-render the LED matrix — while keeping all state on the main thread.

### Display
`GamerHardware.display[col][row]` (UInt8, 0 or 1) is the exact analogue of
`gamer.display[x][y]` in the C++ firmware. `MatrixDisplayView` reads this array
directly via `@ObservedObject`.

### Sound toggle
The physical capacitive-touch pad maps to the **SOUND** button in the UI.
Tapping it toggles `gamer.soundEnabled` and calls `SoundEngine.stopTone()` when
muting, mirroring `checkSoundToggle()` in `TWSUGamerPlus.ino`.

---

## Game controls reference

| Button | Snake | Tetris | Breakout | Simon | Flappy | Dino | Alien | Conway |
|--------|-------|--------|----------|-------|--------|------|-------|--------|
| ▲ UP   | steer up | rotate | – | answer UP | flap | jump | shoot | – |
| ▼ DOWN | steer dn | soft-drop | – | answer DN | – | duck | – | – |
| ◀ LEFT | steer lt | move left | paddle lt | answer LT | – | – | move left | – |
| ▶ RIGHT | steer rt | move right | paddle rt | answer RT | – | – | move right | randomise |
| START  | exit | exit | exit | exit | exit | exit | exit | exit |

---

## License

MIT — see [LICENSE](../../LICENSE) at the repository root.
