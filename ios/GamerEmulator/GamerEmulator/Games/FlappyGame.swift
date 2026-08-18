// FlappyGame.swift — Faithful port of src/games/flappy.h
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

import Foundation

@MainActor
final class FlappyGame: Game {

    let name       = "FLAPPY"
    let gameIndex  = 3
    let animFrames = GameAssets.flappy

    // ── State ──────────────────────────────────────────────────────────────────
    private var menu:              Bool = true
    private var gameOver:          Bool = false
    private var displayScore:      Bool = false
    private var birdPos:           Int  = 2
    private var pipePos:           Int  = 8
    private var pipeGap:           Int  = 3
    private var ticks:             Int  = 0
    private var tickCount:         Int  = 14
    private var score:             Int  = 0

    private var inGameScreen: [UInt8] = Array(repeating: 0, count: 64)
    private var menuScreen:   [UInt8] = [
        0,0,0,1,1,0,0,0,
        0,0,1,0,0,1,0,0,
        0,0,1,0,1,1,0,0,
        1,1,1,0,0,1,0,0,
        1,0,0,1,1,1,1,0,
        1,0,1,0,1,0,0,1,
        0,1,0,0,1,1,1,0,
        0,0,1,1,1,0,0,0,
    ]

    // ── Game protocol ──────────────────────────────────────────────────────────

    func reset(fromHighScore: Bool, startingScore: UInt8) {
        menu  = true
        resetFlappy(fromHighScore: fromHighScore, startingScore: startingScore)
    }

    private func resetFlappy(fromHighScore: Bool, startingScore: UInt8) {
        if fromHighScore && startingScore > 0 {
            score     = Int(startingScore) / 4
            tickCount = max(6, 14 - (score / 7))
        } else {
            score     = 0
            tickCount = 14
        }
        displayScore = false
        ticks        = 0
        birdPos      = 2
        pipePos      = 20
        pipeGap      = 3
        gameOver     = false
        drawInGame(colour: 1)
    }

    func loop(gamer: GamerHardware) async {
        gamer.updateLEDFlash()
        if gamer.soundEnabled { gamer.stopTone() }

        if menu {
            ticks += 1
            if (ticks % 12) == 0 {
                if Int.random(in: 28..<40) % 30 == 0 {
                    menuScreen[19] = 1; menuScreen[20] = 0
                } else {
                    menuScreen[19] = 0; menuScreen[20] = 1
                }
            }
            if gamer.isPressed(.up) {
                menu = false
                resetFlappy(fromHighScore: false, startingScore: 0)
            }
        } else if displayScore {
            gamer.clear()
            gamer.showScore(score / 10, score % 10)
            HighScoreStore.saveHighScore(score, gameIndex)
            await gamer.delay(800)
            displayScore = false
            for i in 0..<64 { inGameScreen[i] = 0 }
            resetFlappy(fromHighScore: false, startingScore: 0)
            menu = true
        } else {
            drawInGame(colour: 0)
            ticks += 1

            if !gameOver && (ticks % tickCount == 0) {
                let lastPipePos = pipePos
                pipePos -= 1
                if pipePos < -1 {
                    score += 1
                    if score % 7 == 0 { tickCount = max(6, tickCount - 1) }
                    gamer.startLEDFlash()
                    pipePos = 7
                    pipeGap = 1 + Int.random(in: 0..<4)
                }

                let lastBirdPos = birdPos
                if gamer.isPressed(.up) {
                    birdPos = max(birdPos - 1, 0)
                    if gamer.soundEnabled { gamer.playTone(NOTE_A8) }
                } else {
                    birdPos += 1
                    if birdPos >= 8 {
                        gameOver = true
                        await gamer.playLossTune()
                        pipePos  = lastPipePos
                        birdPos  = lastBirdPos
                        ticks    = 0
                    }
                }

                if pipePos == 1 || pipePos == 0 { gamer.startLEDFlash() }

                if (pipePos == 1 || pipePos == 0) &&
                   (birdPos < pipeGap || birdPos >= pipeGap + 3) {
                    gameOver = true
                    await gamer.playLossTune()
                    pipePos  = lastPipePos
                    birdPos  = lastBirdPos
                    ticks    = 0
                }
            } else if gameOver && ticks >= 96 {
                displayScore = true
            }

            if !menu { drawInGame(colour: 1) }
        }

        if !displayScore {
            let screen: [UInt8] = menu ? menuScreen : inGameScreen
            for i in 0..<64 {
                let x = i & 7
                let y = i >> 3
                gamer.display[x][y] = screen[i]
            }
            gamer.updateDisplay()
        }

        await gamer.delay(10)
    }

    // MARK: - Helpers

    private func drawInGame(colour: UInt8) {
        if birdPos >= 0 && birdPos < 8 {
            let show = !gameOver || (gameOver && ((ticks / 24) % 2) == 1)
            if show { inGameScreen[1 + birdPos * 8] = colour }
        }
        for y in 0..<8 {
            guard y < pipeGap || y >= pipeGap + 3 else { continue }
            if pipePos >= 0 && pipePos < 8    { inGameScreen[pipePos + y * 8]     = colour }
            if pipePos >= -1 && pipePos < 7   { inGameScreen[pipePos + 1 + y * 8] = colour }
        }
    }
}
