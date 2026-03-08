// TetrisGame.swift — Faithful port of src/games/tetris.h
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

import Foundation

@MainActor
final class TetrisGame: Game {

    let name       = "TETRIS"
    let gameIndex  = 4
    let animFrames = GameAssets.tetris

    // ── State ──────────────────────────────────────────────────────────────────
    private var moveInterval: Double = 1200    // ms
    private var level: Int = 1
    private var linesCleared: Int = 0
    private var gameOver: Bool = false
    private var lastDownPressTime: Double = 0
    private var isInDownPress: Bool = false
    private var score: Int = 0
    private var currentX: Int = 3
    private var currentY: Int = -1

    private var grid: [[Int]] = Array(repeating: Array(repeating: 0, count: 8), count: 8)
    private var piece: [[Int]] = Array(repeating: Array(repeating: 0, count: 3), count: 3)

    private enum PieceType: Int, CaseIterable { case I, O, T, S, Z, J, L }
    private var pieceType: PieceType = .O

    private var lastMoveTime: Double = 0

    // Tetris melody playback
    private var lastNoteTime: Double = 0
    private var noteIdx: Int = 0
    private var chirpPending: Bool = false

    // ── Game protocol ──────────────────────────────────────────────────────────

    func reset(fromHighScore: Bool, startingScore: UInt8) {
        if fromHighScore && startingScore > 0 {
            score        = Int(startingScore) / 2
            level        = 1 + (score / 7)
            linesCleared = (level - 1) * 3
            moveInterval = Double(max(300, 1200 - (level - 1) * 200))
        } else {
            score        = 0
            level        = 1
            linesCleared = 0
            moveInterval = 1200
        }
        gameOver      = false
        isInDownPress = false
        currentX = 3
        currentY = -1
        for i in 0..<8 { for j in 0..<8 { grid[i][j] = 0 } }
        chirpPending  = false
        noteIdx       = 0
        lastNoteTime  = 0
        lastMoveTime  = 0
        spawnPiece()
    }

    func loop(gamer: GamerHardware) async {
        // Melody management (updateGameInput with stopTone=false)
        gamer.updateLEDFlash()

        if gamer.soundEnabled && chirpPending {
            gamer.stopTone()
            chirpPending = false
        }

        if gamer.soundEnabled {
            let now = gamer.millis()
            if now - lastNoteTime >= 200 {
                gamer.playTone(GameAssets.tetrisMelody[noteIdx])
                noteIdx = (noteIdx + 1) % GameAssets.tetrisMelody.count
                lastNoteTime = now
            }
        }

        // Game-over sequence
        if gameOver {
            await gamer.playLossTune()
            HighScoreStore.saveHighScore(min(score, 99), gameIndex)
            gamer.showScore(min(score, 99) / 10, min(score, 99) % 10)
            await gamer.delay(2000)
            reset(fromHighScore: false, startingScore: 0)
            return
        }

        // Gravity
        let now = gamer.millis()
        if now - lastMoveTime > moveInterval {
            lastMoveTime = now
            if canMove(x: currentX, y: currentY + 1) {
                currentY += 1
                gamer.startLEDFlash()
                renderGridAndPiece(gamer: gamer)
            } else {
                // Lock piece
                lockPiece(gamer: gamer)
                if gamer.soundEnabled { gamer.playTone(NOTE_C8); chirpPending = true }
                await checkLines(gamer: gamer)
                spawnPiece(gamer: gamer)
                currentX = 3
                currentY = -3
                if !canMove(x: currentX, y: currentY) || !canMove(x: currentX, y: currentY + 2) {
                    gameOver = true
                }
                await gamer.delay(80)
                renderGridAndPiece(gamer: gamer)
                currentY = -1
            }
        }

        // Button input
        var btnPressed = false
        if gamer.isPressed(.left) && canMove(x: currentX - 1, y: currentY) {
            currentX -= 1; isInDownPress = false; renderGridAndPiece(gamer: gamer)
        } else if gamer.isPressed(.right) && canMove(x: currentX + 1, y: currentY) {
            currentX += 1; isInDownPress = false; renderGridAndPiece(gamer: gamer)
        } else if gamer.isPressed(.down) && canMove(x: currentX, y: currentY + 1) {
            currentY += 1; btnPressed = true; renderGridAndPiece(gamer: gamer)
            isInDownPress = true; lastDownPressTime = now
        } else if gamer.isHeld(.down) && isInDownPress && now - lastDownPressTime >= 300 {
            if canMove(x: currentX, y: currentY + 1) {
                currentY += 1; btnPressed = true; renderGridAndPiece(gamer: gamer)
                lastDownPressTime = now
            }
        } else if gamer.isPressed(.up) {
            isInDownPress = false
            rotatePiece()
            renderGridAndPiece(gamer: gamer)
        } else {
            isInDownPress = false
        }

        if btnPressed { gamer.startLEDFlash() }
        if gamer.soundEnabled && btnPressed { gamer.playTone(NOTE_D8); chirpPending = true }
    }

    // MARK: - Helpers

    private func canMove(x: Int, y: Int, p: [[Int]]? = nil) -> Bool {
        let testPiece = p ?? piece
        for i in 0..<3 {
            for j in 0..<3 {
                guard testPiece[i][j] == 1 else { continue }
                let nx = x + j, ny = y + i
                if nx < 0 || nx >= 8 || ny < -3 || ny >= 8 { return false }
                if ny >= 0 && grid[ny][nx] == 1 { return false }
            }
        }
        return true
    }

    private func renderGridAndPiece(gamer: GamerHardware) {
        for i in 0..<8 { for j in 0..<8 { gamer.display[j][i] = UInt8(grid[i][j]) } }
        for i in 0..<3 {
            for j in 0..<3 {
                guard piece[i][j] == 1 else { continue }
                let x = currentX + j, y = currentY + i
                if x >= 0 && x < 8 && y >= 0 && y < 8 { gamer.display[x][y] = 1 }
            }
        }
        gamer.updateDisplay()
    }

    private func lockPiece(gamer: GamerHardware) {
        for i in 0..<3 {
            for j in 0..<3 {
                guard piece[i][j] == 1 else { continue }
                let x = currentX + j, y = currentY + i
                if x >= 0 && x < 8 && y >= 0 && y < 8 { grid[y][x] = 1 }
            }
        }
    }

    private func checkLines(gamer: GamerHardware) async {
        var cleared = 0
        var i = 0
        while i < 8 {
            var full = true
            for j in 0..<8 { if grid[i][j] == 0 { full = false; break } }
            if full {
                cleared += 1
                if gamer.soundEnabled { gamer.playTone(NOTE_A8); chirpPending = true }
                gamer.startLEDFlash()
                // Animate clear
                for j in 0..<8 {
                    grid[i][j] = 0
                    renderGridAndPiece(gamer: gamer)
                    await gamer.delay(25)
                }
                // Cascade rows down
                for k in stride(from: i, through: 1, by: -1) {
                    for j in 0..<8 { grid[k][j] = grid[k-1][j] }
                }
                for j in 0..<8 { grid[0][j] = 0 }
                linesCleared += 1
                if linesCleared % 7 == 0 {
                    level += 1
                    moveInterval = max(300, moveInterval - 200)
                }
                // Don't increment i – recheck same row after cascade
            } else {
                i += 1
            }
        }
        if cleared > 0 {
            let bonusMultipliers = [0, 1, 3, 5]
            score += level * bonusMultipliers[min(cleared, 3)]
        }
    }

    private func spawnPiece(gamer: GamerHardware? = nil) {
        for i in 0..<3 { for j in 0..<3 { piece[i][j] = 0 } }
        pieceType = PieceType(rawValue: Int.random(in: 0..<7))!
        switch pieceType {
        case .I: piece[1][0]=1; piece[1][1]=1; piece[1][2]=1
        case .O: piece[0][0]=1; piece[0][1]=1; piece[1][0]=1; piece[1][1]=1
        case .T: piece[0][1]=1; piece[1][0]=1; piece[1][1]=1; piece[1][2]=1
        case .S: piece[0][1]=1; piece[0][2]=1; piece[1][0]=1; piece[1][1]=1
        case .Z: piece[0][0]=1; piece[0][1]=1; piece[1][1]=1; piece[1][2]=1
        case .J: piece[0][0]=1; piece[1][0]=1; piece[1][1]=1; piece[1][2]=1
        case .L: piece[0][2]=1; piece[1][0]=1; piece[1][1]=1; piece[1][2]=1
        }
        if let g = gamer { if g.soundEnabled { g.playTone(NOTE_G8); chirpPending = true } }
    }

    private func rotatePiece() {
        guard pieceType != .O else { return }
        var temp = Array(repeating: Array(repeating: 0, count: 3), count: 3)
        for i in 0..<3 { for j in 0..<3 { temp[j][2-i] = piece[i][j] } }
        if canMove(x: currentX, y: currentY, p: temp) { piece = temp }
    }
}
