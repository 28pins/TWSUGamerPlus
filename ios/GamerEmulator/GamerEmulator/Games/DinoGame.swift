// DinoGame.swift — Faithful port of src/games/dino.h
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

import Foundation

@MainActor
final class DinoGame: Game {

    let name       = "DINO"
    let gameIndex  = 7
    let animFrames = GameAssets.dino

    // ── State ──────────────────────────────────────────────────────────────────
    private var dinoY:        Int    = 5
    private var dinoVel:      Int    = 0
    private var dinoJumping:  Bool   = false
    private var dinoDucking:  Bool   = false
    private var obsX:         Int    = 9
    private var obsType:      Int    = 0   // 0=cactus 1=bird 2=giant-bird 3=wide-cactus 4=flying-bar
    private var score:        Int    = 0
    private var dinoOver:     Bool   = false
    private var dinoSpeed:    Int    = 160 // ms per tick
    private var lastTick:     Double = 0

    // ── Game protocol ──────────────────────────────────────────────────────────

    func reset(fromHighScore: Bool, startingScore: UInt8) {
        dinoY       = 5
        dinoVel     = 0
        dinoJumping = false
        dinoDucking = false
        obsX        = 9
        obsType     = 0
        dinoOver    = false
        if fromHighScore && startingScore > 0 {
            score     = Int(startingScore) / 2
            dinoSpeed = max(60, 160 - score * 5)
        } else {
            score     = 0
            dinoSpeed = 160
        }
        lastTick = 0
    }

    func loop(gamer: GamerHardware) async {
        gamer.updateLEDFlash()
        if gamer.soundEnabled { gamer.stopTone() }

        if dinoOver {
            gamer.showScore(score / 10, score % 10)
            HighScoreStore.saveHighScore(score, gameIndex)
            await gamer.delay(2000)
            reset(fromHighScore: false, startingScore: 0)
            return
        }

        // Input
        if gamer.isPressed(.up) && !dinoJumping {
            dinoJumping = true; dinoDucking = false; dinoVel = -2
            gamer.startLEDFlash()
            if gamer.soundEnabled { gamer.playTone(NOTE_A8) }
        }
        if gamer.isHeld(.down) {
            if dinoJumping { dinoY = 5; dinoVel = 0; dinoJumping = false }
            if !dinoDucking { gamer.startLEDFlash() }
            dinoDucking = true
        } else if !dinoJumping {
            dinoDucking = false
        }

        // Tick-gated update
        let now = gamer.millis()
        if lastTick == 0 { lastTick = now }

        if now - lastTick >= Double(dinoSpeed) {
            lastTick = now

            // Jump physics
            if dinoJumping {
                dinoY   += dinoVel
                dinoVel += 1
                if dinoY >= 5 { dinoY = 5; dinoVel = 0; dinoJumping = false }
            }

            // Advance obstacle
            obsX -= 1
            if obsX < 0 {
                obsX = Int.random(in: 8..<11)
                let roll = Int.random(in: 0..<5)
                switch roll {
                case 0: obsType = 1
                case 1: obsType = 2
                case 2: obsType = 3
                case 3: obsType = 4
                default: obsType = 0
                }
                score += 1
                if score > 99 { score = 99 }
                gamer.startLEDFlash()
                if gamer.soundEnabled { gamer.playTone(NOTE_E8) }
                if dinoSpeed > 60 { dinoSpeed -= 4 }
            }

            // Collision at column 1
            if obsX == 1 { if collisionCheck() { triggerGameOver(gamer: gamer); return } }
            if obsX == 0 && obsType == 3 { if collisionCheck() { triggerGameOver(gamer: gamer); return } }
            if (obsX == 1 || obsX == 0 || obsX == -1) && obsType == 4 {
                if collisionCheck4() { triggerGameOver(gamer: gamer); return }
            }
        }

        // Render
        for i in 0..<8 { for j in 0..<8 { gamer.display[i][j] = 0 } }
        for i in 0..<8 { gamer.display[i][7] = 1 }

        if dinoDucking {
            gamer.display[1][6] = 1
        } else {
            if dinoY   >= 0 && dinoY   < 8 { gamer.display[1][dinoY]     = 1 }
            if dinoY+1 >= 0 && dinoY+1 < 8 { gamer.display[1][dinoY + 1] = 1 }
        }

        drawObstacle(gamer: gamer)

        gamer.updateDisplay()
        await gamer.delay(10)
    }

    // MARK: - Helpers

    private func collisionCheck() -> Bool {
        switch obsType {
        case 0: return dinoDucking || dinoY + 1 >= 5
        case 1: return !dinoDucking && dinoY <= 5 && dinoY + 1 >= 5
        case 2: return !dinoDucking || dinoJumping
        case 3: return dinoDucking || dinoY + 1 >= 5
        case 4: return !dinoDucking && dinoY <= 5 && dinoY + 1 >= 5
        default: return false
        }
    }

    private func collisionCheck4() -> Bool {
        return !dinoDucking && dinoY <= 5 && dinoY + 1 >= 5
    }

    private func triggerGameOver(gamer: GamerHardware) {
        dinoOver = true
        Task { await gamer.playLossTune() }
    }

    private func drawObstacle(gamer: GamerHardware) {
        if obsX >= 0 && obsX < 8 {
            switch obsType {
            case 0:
                gamer.display[obsX][5] = 1; gamer.display[obsX][6] = 1
            case 1:
                gamer.display[obsX][5] = 1
            case 2:
                gamer.display[obsX][5] = 1; gamer.display[obsX][4] = 1; gamer.display[obsX][3] = 1
            case 3:
                gamer.display[obsX][5] = 1; gamer.display[obsX][6] = 1
                if obsX + 1 < 8 { gamer.display[obsX+1][5] = 1; gamer.display[obsX+1][6] = 1 }
            case 4:
                gamer.display[obsX][5] = 1
                if obsX+1 < 8 { gamer.display[obsX+1][5] = 1 }
                if obsX+2 < 8 { gamer.display[obsX+2][5] = 1 }
            default: break
            }
        }
        if obsX == -1 && obsType == 3 { gamer.display[0][5] = 1; gamer.display[0][6] = 1 }
        if obsX == -1 && obsType == 4 { gamer.display[0][5] = 1; gamer.display[1][5] = 1 }
        if obsX == -2 && obsType == 4 { gamer.display[0][5] = 1 }
    }
}
