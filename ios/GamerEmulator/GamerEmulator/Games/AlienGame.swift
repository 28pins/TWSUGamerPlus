// AlienGame.swift — Faithful port of src/games/alien.h
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

import Foundation

@MainActor
final class AlienGame: Game {

    let name       = "ALIEN"
    let gameIndex  = 5
    let animFrames = GameAssets.alien

    // ── State ──────────────────────────────────────────────────────────────────
    private var gameGoing:     Bool   = false
    private var lastMove:      Double = 0
    private var moveDelay:     Int    = 800
    private var alienLineCount: Int   = 0
    private var score:         Int    = 0
    private var playerX:       Int    = 3

    // ── Game protocol ──────────────────────────────────────────────────────────

    func reset(fromHighScore: Bool, startingScore: UInt8) {
        if fromHighScore && startingScore > 0 {
            score          = Int(startingScore) / 2
            alienLineCount = score * 10 - 1
            moveDelay      = max(330, 800 - score * 5)
        } else {
            score          = 0
            alienLineCount = 0
            moveDelay      = 800
        }
        playerX   = 3
        lastMove  = 0
        gameGoing = true
    }

    func loop(gamer: GamerHardware) async {
        if gameGoing {
            await moveAliens(gamer: gamer)

            // Inner wait loop: handle input during move delay
            while gamer.millis() - lastMove < Double(moveDelay) {
                await gamer.delay(10)
                gamer.updateLEDFlash()
                if gamer.soundEnabled { gamer.stopTone() }

                if gamer.isPressed(.up) {
                    // Shoot upward
                    for i in stride(from: 7, through: 0, by: -1) {
                        gamer.display[playerX][i] = 1
                        await gamer.delay(20)
                        renderPlayer(gamer: gamer)
                        gamer.updateDisplay()
                    }
                    for i in stride(from: 7, through: 0, by: -1) {
                        gamer.display[playerX][i] = 0
                        await gamer.delay(20)
                        renderPlayer(gamer: gamer)
                        gamer.updateDisplay()
                    }
                    lastMove += 200
                }
                if gamer.isPressed(.left) && playerX > 0 {
                    playerX -= 1; renderPlayer(gamer: gamer); gamer.updateDisplay()
                }
                if gamer.isPressed(.right) && playerX < 7 {
                    playerX += 1; renderPlayer(gamer: gamer); gamer.updateDisplay()
                }
            }
        } else {
            // Game over state
            if gamer.isPressed(.up) || gamer.isPressed(.left) || gamer.isPressed(.right) {
                reset(fromHighScore: false, startingScore: 0)
            } else {
                gamer.showScore(score / 10, score % 10)
                await gamer.delay(1000)
                reset(fromHighScore: false, startingScore: 0)
            }
        }
    }

    // MARK: - Helpers

    private func renderPlayer(gamer: GamerHardware) {
        for i in 0..<8 { gamer.display[i][7] = 0; gamer.display[i][6] = 0 }
        gamer.display[playerX][7] = 1
        gamer.display[playerX][6] = 1
        if playerX > 0 { gamer.display[playerX - 1][7] = 1 }
        if playerX < 7 { gamer.display[playerX + 1][7] = 1 }
    }

    private func generateAlienRow(gamer: GamerHardware) {
        for i in 0..<8 {
            gamer.display[i][0] = Int.random(in: 0..<(moveDelay > 330 ? 3 : 4)) > 1 ? 1 : 0
        }
    }

    private func moveAliens(gamer: GamerHardware) async {
        // Check for loss: alien in row 5
        for i in 0..<8 {
            if gamer.display[i][5] == 1 {
                gameGoing = false
                HighScoreStore.saveHighScore(score, gameIndex)
                return
            }
        }
        // Shift rows down
        for i in 0..<8 {
            for j in stride(from: 5, through: 1, by: -1) {
                gamer.display[i][j] = gamer.display[i][j - 1]
            }
        }
        generateAlienRow(gamer: gamer)
        renderPlayer(gamer: gamer)
        gamer.updateDisplay()
        lastMove = gamer.millis()

        alienLineCount += 1
        if alienLineCount % 10 == 0 { score += 1 }
        if moveDelay > 300 { moveDelay -= 5 }
    }
}
