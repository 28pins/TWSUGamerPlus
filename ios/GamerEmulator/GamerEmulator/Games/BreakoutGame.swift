// BreakoutGame.swift — Faithful port of src/games/breakout.h
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

import Foundation

@MainActor
final class BreakoutGame: Game {

    let name       = "BRKOUT"
    let gameIndex  = 1
    let animFrames = GameAssets.breakout

    // ── State ──────────────────────────────────────────────────────────────────
    private var ballX: Int = 5
    private var ballY: Int = 5
    private var velX: Int = -1
    private var velY: Int = -1
    private var origVX: Int = -1
    private var origVY: Int = -1
    private var blocks: [[Int]] = Array(repeating: Array(repeating: 0, count: 8), count: 8)
    private var paddleX: Int = 2
    private var counter: Int = 0
    private var score: Int = 0

    // ── Game protocol ──────────────────────────────────────────────────────────

    func reset(fromHighScore: Bool, startingScore: UInt8) {
        resetBlocks()
        if fromHighScore && startingScore > 0 {
            score = Int(startingScore) / 4
        } else {
            score = 0
        }
        ballX = Int.random(in: 4..<8)
        ballY = 5
        velX  = -1
        velY  = -1
        paddleX = 2
        counter = 0
    }

    func loop(gamer: GamerHardware) async {
        gamer.updateLEDFlash()
        if gamer.soundEnabled { gamer.stopTone() }

        if counter > 2 {
            // Clear ball region
            for x in 0..<8 { for y in 0..<8 { gamer.display[x][y] = 0 } }
        }

        // Clear bottom row for paddle redraw
        for x in 0..<8 { gamer.display[x][7] = 0 }

        // Paddle input
        if gamer.isHeld(.left)  && paddleX > -3 { paddleX -= 1 }
        else if gamer.isHeld(.right) && paddleX < 7  { paddleX += 1 }

        // Draw paddle (4 wide)
        for a in 0..<4 {
            let px = paddleX + a
            if px >= 0 && px < 8 { gamer.display[px][7] = 1 }
        }

        if counter > 2 {
            origVX = velX; origVY = velY
            // Draw blocks
            for x in 0..<8 { for y in 0..<4 { if blocks[x][y] == 1 { gamer.display[x][y] = 1 } } }

            physics(gamer: gamer)

            // Propagate block destruction
            for x in 0..<8 {
                for y in 0..<8 {
                    if blocks[x][y] == 0 {
                        let adjX = ((x % 2) == (y % 2)) ? ((x < 7) ? x + 1 : 0) : ((x > 0) ? x - 1 : 7)
                        blocks[adjX][y] = 0
                    }
                }
            }
            for x in 0..<8 { for y in 0..<4 { if blocks[x][y] == 0 { gamer.display[x][y] = 0 } } }

            // Secondary boundary logic (mirrors original)
            let newX = ballX + velX
            let newY = ballY + velY
            if newX > -1 && newX < 8 {
                if !(newY > -1 && newY < 8) {
                    if gamer.display[newX][ballY - velY] == 0 {
                        blocks[newX][ballY + velY] = 0
                        velY *= -1
                    } else {
                        blocks[ballX + velX][ballY + velY] = 0
                        velY *= -1; velX *= -1
                    }
                }
            } else {
                if gamer.display[ballX - velX][newY] == 0 {
                    blocks[ballX + velX][newY] = 0
                    velX *= -1
                    if !(newY > -1 && newY < 8) {
                        if gamer.display[ballX + velX][ballY - velY] == 0 {
                            blocks[ballX - velX][ballY - velY] = 0
                            velY *= -1
                        }
                    }
                } else {
                    for dx in -1...1 { for dy in -1...1 {
                        let bx = ballX + dx, by = ballY + dy
                        if bx >= 0 && bx < 8 && by >= 0 && by < 8 { blocks[bx][by] = 0 }
                    }}
                    velX *= -1; velY *= -1
                }
            }

            ballX += velX; ballY += velY
            if ballX >= 0 && ballX < 8 && ballY >= 0 && ballY < 8 {
                gamer.display[ballX][ballY] = 1
            }
            counter = 0
        } else {
            counter += 1
        }

        gamer.updateDisplay()

        // Ball lost
        if ballY == 7 {
            for _ in 0..<4 {
                gamer.clear()
                await gamer.delay(150)
                if ballX >= 0 && ballX < 8 { gamer.display[ballX][ballY] = 1 }
                gamer.updateDisplay()
                await gamer.delay(150)
            }
            gamer.clear()
            HighScoreStore.saveHighScore(score, gameIndex)
            if score > 0 { gamer.showScore(score / 10, score % 10) }
            await gamer.delay(500)
            reset(fromHighScore: false, startingScore: 0)
            return
        }

        // Win – all blocks cleared
        var finished = true
        outer: for x in 0..<8 { for y in 0..<4 { if blocks[x][y] == 1 { finished = false; break outer } } }
        if finished {
            score += 1
            gamer.clear()
            HighScoreStore.saveHighScore(score, gameIndex)
            await gamer.delay(500)
            resetBlocks()
            ballX = Int.random(in: 4..<8); ballY = 5; velX = -1; velY = -1
        }

        await gamer.delay(50)
    }

    // MARK: - Helpers

    private func resetBlocks() {
        for x in 0..<8 { for y in 0..<8 { blocks[x][y] = 0 } }
        for x in 0..<8 { for y in 0..<4 { blocks[x][y] = 1 } }
    }

    private func outOfBounds(_ x: Int, _ y: Int) -> Bool {
        x >= 8 || x < 0 || y >= 8 || y < 0
    }

    private func isFree(_ x: Int, _ y: Int, gamer: GamerHardware) -> Bool {
        !outOfBounds(x, y) && gamer.display[x][y] == 0
    }

    private func physics(gamer: GamerHardware) {
        let nextX = ballX + velX
        let nextY = ballY + velY

        guard !outOfBounds(nextX, nextY) && gamer.display[nextX][nextY] == 0 else {
            // Collision
            gamer.startLEDFlash()
            if gamer.soundEnabled {
                gamer.playTone(ballY == 6 ? NOTE_C8 : NOTE_E8)
            }
            let canBounceY = isFree(nextX, ballY - velY, gamer: gamer)
            let canBounceX = isFree(ballX - velX, nextY, gamer: gamer)
            if canBounceY { velY *= -1 }
            else if canBounceX { velX *= -1 }
            else { velX *= -1; velY *= -1 }

            if !outOfBounds(ballX + origVX, ballY + origVY) {
                blocks[ballX + origVX][ballY + origVY] = 0
            }
            return
        }
    }
}
