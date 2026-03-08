// ConwayGame.swift — Faithful port of src/games/conway.h
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

import Foundation

@MainActor
final class ConwayGame: Game {

    let name       = "CONWAY"
    let gameIndex  = 6
    let animFrames = GameAssets.conway

    private static let stagnationLimit = 5

    // ── State ──────────────────────────────────────────────────────────────────
    private var curr:  [UInt8] = Array(repeating: 0, count: 8)
    private var next_: [UInt8] = Array(repeating: 0, count: 8)
    private var stuck: Int = 0

    // ── Game protocol ──────────────────────────────────────────────────────────

    func reset(fromHighScore: Bool, startingScore: UInt8) {
        randomize()
    }

    func loop(gamer: GamerHardware) async {
        gamer.updateLEDFlash()
        if gamer.soundEnabled { gamer.stopTone() }

        let changed = step()
        if !changed {
            stuck += 1
            if stuck > Self.stagnationLimit { randomize() }
        } else {
            stuck = 0
        }

        if gamer.isPressed(.right) { randomize() }

        for x in 0..<8 { for y in 0..<8 { gamer.display[x][y] = (curr[y] >> x) & 1 } }
        gamer.updateDisplay()
        await gamer.delay(180)
    }

    // MARK: - Conway logic

    func randomize() {
        for i in 0..<8 { curr[i] = UInt8.random(in: 0...255) }
        stuck = 0
    }

    @discardableResult
    func step() -> Bool {
        var anyChange = false
        for y in 0..<8 {
            next_[y] = 0
            for x in 0..<8 {
                var alive: Int = 0
                for dy in -1...1 {
                    for dx in -1...1 {
                        if dx == 0 && dy == 0 { continue }
                        let nx = (x + dx + 8) & 7
                        let ny = (y + dy + 8) & 7
                        if (curr[ny] >> nx) & 1 == 1 { alive += 1 }
                    }
                }
                let isCurr = (curr[y] >> x) & 1 == 1
                let isNext = alive == 3 || (isCurr && alive == 2)
                if isNext { next_[y] |= (1 << x) }
                if isNext != isCurr { anyChange = true }
            }
        }
        for i in 0..<8 { curr[i] = next_[i] }
        return anyChange
    }

    /// Restricted 4×4 step used by the launcher animation preview.
    @discardableResult
    func stepSmall() -> Bool {
        var anyChange = false
        for y in 2..<6 {
            next_[y] = curr[y]
            for x in 2..<6 {
                var alive = 0
                for dy in -1...1 {
                    for dx in -1...1 {
                        if dx == 0 && dy == 0 { continue }
                        var nx = (x - 2 + dx); if nx < 0 { nx += 4 }; if nx >= 4 { nx -= 4 }; nx += 2
                        var ny = (y - 2 + dy); if ny < 0 { ny += 4 }; if ny >= 4 { ny -= 4 }; ny += 2
                        if (curr[ny] >> nx) & 1 == 1 { alive += 1 }
                    }
                }
                let isCurr = (curr[y] >> x) & 1 == 1
                let isNext = alive == 3 || (isCurr && alive == 2)
                if isNext { next_[y] |= (1 << x) }
                else { next_[y] &= ~(1 << x) }
                if isNext != isCurr { anyChange = true }
            }
        }
        for i in 2..<6 { curr[i] = next_[i] }
        return anyChange
    }

    /// Access the current grid for launcher preview rendering.
    func getCurr() -> [UInt8] { curr }
}
