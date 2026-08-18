// SnakeGame.swift — Faithful port of src/games/snake.h
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

import Foundation

@MainActor
final class SnakeGame: Game {

    let name       = "SNAKE"
    let gameIndex  = 0
    let animFrames = GameAssets.snake

    // ── State ──────────────────────────────────────────────────────────────────
    private var dir: Int = 1          // 1=up 2=right 3=down 4=left
    private var goalX: Int = 0
    private var goalY: Int = 0
    private var snakeMap: [[Int]] = Array(repeating: Array(repeating: 0, count: 8), count: 8)
    private var snakeLength: Int = 2
    private var score: Int = 0
    private var currentX: Int = 0
    private var currentY: Int = 0

    // ── Game protocol ──────────────────────────────────────────────────────────

    func reset(fromHighScore: Bool, startingScore: UInt8) {
        if fromHighScore && startingScore > 0 {
            score       = Int(startingScore) / 4
            snakeLength = min(6, 2 + score)
        } else {
            snakeLength = 2
            score       = 0
        }
        dir      = 1
        currentX = 0
        currentY = 0
        for x in 0..<8 { for y in 0..<8 { snakeMap[x][y] = 0 } }
        repeat {
            goalX = Int.random(in: 0..<8)
            goalY = Int.random(in: 0..<8)
        } while goalX == 0 && goalY == 0
    }

    func loop(gamer: GamerHardware) async {
        // Common preamble
        gamer.updateLEDFlash()
        if gamer.soundEnabled { gamer.stopTone() }

        // Clear display
        for x in 0..<8 { for y in 0..<8 { gamer.display[x][y] = 0 } }

        // Input
        var moved = false
        if gamer.isPressed(.up)    && dir != 3 { dir = 1; moved = true }
        if gamer.isPressed(.right) && dir != 4 { dir = 2; moved = true }
        if gamer.isPressed(.down)  && dir != 1 { dir = 3; moved = true }
        if gamer.isPressed(.left)  && dir != 2 { dir = 4; moved = true }
        if gamer.soundEnabled && moved { gamer.playTone(NOTE_E8) }

        // Move head
        switch dir {
        case 1: currentY = (currentY - 1 + 8) & 7
        case 2: currentX = (currentX + 1) & 7
        case 3: currentY = (currentY + 1) & 7
        case 4: currentX = (currentX - 1 + 8) & 7
        default: break
        }

        gamer.display[currentX][currentY] = 1

        // Age the snake map, check collision, update head
        for x in 0..<8 { for y in 0..<8 { if snakeMap[x][y] > 0 { snakeMap[x][y] -= 1 } } }

        // Self-collision
        if snakeMap[currentX][currentY] > 0 {
            gamer.clear()
            await gamer.delay(20)
            await gamer.playLossTune()
            HighScoreStore.saveHighScore(score, gameIndex)
            gamer.showScore(score / 10, score % 10)
            await gamer.delay(800)
            reset(fromHighScore: false, startingScore: 0)
            return
        }

        snakeMap[currentX][currentY] = snakeLength

        for x in 0..<8 { for y in 0..<8 { if snakeMap[x][y] > 0 { gamer.display[x][y] = 1 } } }

        // Food collection
        if currentX == goalX && currentY == goalY {
            repeat {
                goalX = Int.random(in: 0..<8)
                goalY = Int.random(in: 0..<8)
            } while snakeMap[goalX][goalY] > 0

            snakeLength += 1
            score = snakeLength - 2
            if gamer.soundEnabled { gamer.playTone(NOTE_A8) }
            gamer.startLEDFlash()

            for x in 0..<8 { for y in 0..<8 { snakeMap[x][y] += 1 } }
        } else {
            gamer.display[goalX][goalY] = 1
        }

        await gamer.delay(80)
        gamer.updateDisplay()
    }
}
