// SimonGame.swift — Faithful port of src/games/simon.h
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

import Foundation

@MainActor
final class SimonGame: Game {

    let name       = "SIMON"
    let gameIndex  = 2
    let animFrames = GameAssets.simon

    private static let maxSeq   = 30
    private static let numDirs  = 4
    private static let notes    = [NOTE_E8, NOTE_C8, NOTE_G8, NOTE_D8]

    // ── State ──────────────────────────────────────────────────────────────────
    private var simonStep: Int = 0
    private var delayMils: Int = 300
    private var sequence: [Int] = Array(repeating: 0, count: 30)
    private var score: Int = 0

    // ── Game protocol ──────────────────────────────────────────────────────────

    func reset(fromHighScore: Bool, startingScore: UInt8) {
        gamer_clear_pending = true
        if fromHighScore && startingScore > 0 {
            simonStep = min(7, Int(startingScore) / 3)
            for b in 0..<simonStep { sequence[b] = Int.random(in: 0..<Self.numDirs) }
            delayMils = max(20, 300 - simonStep * 4)
        } else {
            simonStep = 0
            delayMils = 300
        }
        score = 0
    }

    // Track whether we need a gamer.clear() on the next loop call (replaces reset's blocking delay)
    private var gamer_clear_pending = false

    func loop(gamer: GamerHardware) async {
        gamer.updateLEDFlash()
        if gamer.soundEnabled { gamer.stopTone() }

        if gamer_clear_pending { gamer.clear(); await gamer.delay(100); gamer_clear_pending = false }

        // Guard: if the player has reached the maximum sequence length, celebrate the win.
        // simonStep == maxSeq means all 30 steps were completed successfully.
        if simonStep >= Self.maxSeq {
            score = simonStep
            HighScoreStore.saveHighScore(score, gameIndex)
            await gamer.playWinTune()
            gamer.printImage(GameAssets.simonRight)
            await gamer.delay(600)
            reset(fromHighScore: false, startingScore: 0)
            return
        }

        // Add a new item to the sequence (safe: simonStep is now guaranteed < maxSeq)
        sequence[simonStep] = Int.random(in: 0..<Self.numDirs)

        if simonStep > 0 {
            // Countdown
            for p in stride(from: 3, through: 1, by: -1) {
                if gamer.isHeld(.start) { return }
                gamer.showScore(0, p)
                await gamer.delay(delayMils)
            }
            // "GO"
            if gamer.isHeld(.start) { return }
            gamer.printImage(GameAssets.simonGo)
            await gamer.delay(delayMils)

            // Play sequence
            for i in 0..<simonStep {
                if gamer.isHeld(.start) { return }
                if gamer.soundEnabled { gamer.playTone(Self.notes[sequence[i]]) }
                gamer.printImage(GameAssets.simonArrows[sequence[i]])
                gamer.startLEDFlash()
                await gamer.delay(delayMils)
                if gamer.soundEnabled { gamer.stopTone() }
                gamer.clear()
                await gamer.delay(delayMils)
            }
            gamer.clear()

            // Await player input
            var success = true
            for count in 0..<simonStep {
                if gamer.isHeld(.start) { return }
                var key = 4
                while key == 4 {
                    if gamer.isHeld(.start) { return }
                    if gamer.isHeld(.up)    { key = 0 }
                    if gamer.isHeld(.down)  { key = 1 }
                    if gamer.isHeld(.left)  { key = 2 }
                    if gamer.isHeld(.right) { key = 3 }
                    if key == 4 { await gamer.delay(10) }
                }
                gamer.startLEDFlash()
                gamer.printImage(GameAssets.simonArrows[key])
                if key != sequence[count] {
                    success = false
                    break
                }
                // Wait for button release
                while gamer.isHeld(.up) || gamer.isHeld(.down) ||
                      gamer.isHeld(.left) || gamer.isHeld(.right) {
                    if gamer.isHeld(.start) { return }
                    await gamer.delay(10)
                }
            }

            delayMils -= delayMils / 40
            await gamer.delay(delayMils)

            if success {
                simonStep += 1
                score = simonStep
                await gamer.playWinTune()
                gamer.printImage(GameAssets.simonRight)
            } else {
                score = simonStep
                await gamer.playLossTune()
                HighScoreStore.saveHighScore(score, gameIndex)
                gamer.printImage(GameAssets.simonWrong)
                await gamer.delay(400)
                gamer.showScore((simonStep - 1) / 10, (simonStep - 1) % 10)
                await gamer.delay(400)
                reset(fromHighScore: false, startingScore: 0)
            }
        } else {
            simonStep += 1
            if simonStep > 28 {
                score = simonStep
                simonStep = 28
                await gamer.playWinTune()
            }
        }

        await gamer.delay(400)
    }
}
