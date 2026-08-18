// GamerHardware.swift — Observable emulation of the TWSU Gamer hardware.
// Mirrors the public interface of Gamer.h / Gamer.cpp.
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

import Foundation
import Combine

// Button indices – match #define constants in Gamer.h
enum GamerButton: Int, CaseIterable {
    case up    = 0
    case left  = 1
    case right = 2
    case down  = 3
    case start = 4
}

@MainActor
final class GamerHardware: ObservableObject {

    // MARK: - Published display state

    /// Column-major pixel buffer: display[x][y], x=col 0-7, y=row 0-7.
    @Published var display: [[UInt8]] = Array(
        repeating: Array(repeating: 0, count: 8), count: 8)

    /// Red indicator LED on/off.
    @Published var ledOn: Bool = false

    /// User-set brightness 1-8.
    @Published var brightness: Int = 8

    /// LED compensation enabled flag (affects display only – not true hw boost in emulator).
    @Published var ledCompensation: Bool = true

    /// Sound enabled (toggled via cap-touch pad).
    @Published var soundEnabled: Bool = false

    // MARK: - Private state

    private var buttonHeld: [Bool]  = Array(repeating: false, count: 5)
    private var buttonQueue: [Bool] = Array(repeating: false, count: 5)

    private var ledFlashStart: Date? = nil
    private var lastCapTouchState: Bool = false

    private let soundEngine = SoundEngine()

    // MARK: - Display

    /// Convert display[][] to row-image bitmap (same as updateDisplay() in Gamer.cpp).
    /// In the emulator the display array IS the source of truth – call this after
    /// every frame write so the SwiftUI view picks up the change.
    func updateDisplay() {
        objectWillChange.send()
    }

    func clear() {
        for x in 0..<8 { for y in 0..<8 { display[x][y] = 0 } }
        objectWillChange.send()
    }

    func allOn() {
        for x in 0..<8 { for y in 0..<8 { display[x][y] = 1 } }
        objectWillChange.send()
    }

    /// Print an 8-byte image (one byte per row, bit 7 = column 0).
    /// Matches Gamer::printImage() logic:
    ///   display[i][j] = (img[j] >> (7-i)) & 1
    func printImage(_ img: [UInt8]) {
        for j in 0..<8 {
            for i in 0..<8 {
                display[i][j] = (img[j] >> (7 - i)) & 1
            }
        }
        objectWillChange.send()
    }

    /// Show a two-digit score using the 3-pixel-wide number bitmaps.
    /// Matches showScore() in TWSUGamerPlus.ino.
    func showScore(_ dig1: Int, _ dig2: Int) {
        var result = [UInt8](repeating: 0, count: 8)
        let d1 = max(0, min(9, dig1))
        let d2 = max(0, min(9, dig2))
        for p in 0..<8 {
            result[p] = (GameAssets.numbers[d1][p] << 5) | GameAssets.numbers[d2][p]
        }
        printImage(result)
    }

    // MARK: - Input

    /// Returns true once per physical press (edge detection, clears flag).
    func isPressed(_ button: Int) -> Bool {
        guard button >= 0 && button < 5 else { return false }
        if buttonQueue[button] {
            buttonQueue[button] = false
            return true
        }
        return false
    }

    func isPressed(_ button: GamerButton) -> Bool { isPressed(button.rawValue) }

    /// Returns true while the button is physically held.
    func isHeld(_ button: Int) -> Bool {
        guard button >= 0 && button < 5 else { return false }
        return buttonHeld[button]
    }

    func isHeld(_ button: GamerButton) -> Bool { isHeld(button.rawValue) }

    // MARK: - UI → hardware bridge

    func buttonDown(_ button: GamerButton) {
        buttonHeld[button.rawValue]  = true
        buttonQueue[button.rawValue] = true
    }

    func buttonUp(_ button: GamerButton) {
        buttonHeld[button.rawValue] = false
    }

    /// Resets all button held and queued state (call when transitioning between screens).
    func clearAllButtons() {
        for i in 0..<5 {
            buttonHeld[i]  = false
            buttonQueue[i] = false
        }
    }

    /// Called when the user taps the cap-touch pad — toggles sound.
    func capTouchTap() {
        soundEnabled.toggle()
        if !soundEnabled { soundEngine.stopTone() }
    }

    // MARK: - LED flash

    func startLEDFlash() {
        ledOn = true
        ledFlashStart = Date()
    }

    /// Must be called each game loop frame to auto-extinguish the LED after 175 ms.
    func updateLEDFlash() {
        guard ledOn, let start = ledFlashStart,
              Date().timeIntervalSince(start) >= 0.175 else { return }
        ledOn = false
        ledFlashStart = nil
    }

    func setLED(_ on: Bool) { ledOn = on }

    // MARK: - Sound

    func playTone(_ note: Int) {
        guard soundEnabled else { return }
        soundEngine.playTone(note: note)
    }

    func stopTone() {
        soundEngine.stopTone()
    }

    // MARK: - Brightness

    func setBrightness(_ level: Int) {
        brightness = max(1, min(8, level))
    }

    func getBrightness() -> Int { brightness }
    func getLEDCompensation() -> Bool { ledCompensation }
    func setLEDCompensation(_ enabled: Bool) { ledCompensation = enabled }

    // MARK: - Timing helpers (replace Arduino millis() and delay())

    /// Current time in milliseconds (wall-clock uptime).
    func millis() -> Double {
        ProcessInfo.processInfo.systemUptime * 1000
    }

    /// Async non-blocking delay — replaces Arduino delay().
    func delay(_ milliseconds: Int) async {
        guard milliseconds > 0 else { return }
        try? await Task.sleep(nanoseconds: UInt64(milliseconds) * 1_000_000)
    }

    // MARK: - Shared game helpers (ported from TWSUGamerPlus.ino)

    func playWinTune() async {
        guard soundEnabled else { return }
        let notes = [NOTE_C8, NOTE_E8, NOTE_G8, NOTE_B8]
        for note in notes {
            playTone(note)
            await delay(100)
        }
        stopTone()
    }

    func playLossTune() async {
        guard soundEnabled else { return }
        let notes = [NOTE_B8, NOTE_G8, NOTE_E8, NOTE_B7]
        for note in notes {
            playTone(note)
            await delay(180)
        }
        stopTone()
    }
}
