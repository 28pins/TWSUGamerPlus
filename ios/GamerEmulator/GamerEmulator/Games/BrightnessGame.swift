// BrightnessGame.swift — Faithful port of src/games/brightness.h
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

import Foundation

@MainActor
final class BrightnessGame: Game {

    let name       = "BRIGHT"
    let gameIndex  = 8
    let animFrames = GameAssets.brightness

    private var brightSetting:   Int  = 8
    private var ledCompSetting:  Bool = true
    private var needsInitialDraw: Bool = true

    // ── Game protocol ──────────────────────────────────────────────────────────

    func reset(fromHighScore: Bool, startingScore: UInt8) {
        // Mark that we need to render the initial brightness screen on next loop
        needsInitialDraw = true
    }

    func loop(gamer: GamerHardware) async {
        // On first loop after reset, sync from gamer and draw once
        if needsInitialDraw {
            brightSetting  = gamer.getBrightness()
            ledCompSetting = gamer.getLEDCompensation()
            drawScreen(gamer: gamer)
            needsInitialDraw = false
        } else {
            brightSetting  = gamer.getBrightness()
            ledCompSetting = gamer.getLEDCompensation()
        }

        var changed = false

        if gamer.isPressed(.up) || gamer.isPressed(.right) {
            if brightSetting < 8 { brightSetting += 1; gamer.setBrightness(brightSetting); changed = true }
        } else if gamer.isPressed(.down) {
            if brightSetting > 1 { brightSetting -= 1; gamer.setBrightness(brightSetting); changed = true }
        } else if gamer.isPressed(.left) {
            ledCompSetting.toggle(); gamer.setLEDCompensation(ledCompSetting); changed = true
        }

        if changed { drawScreen(gamer: gamer) }

        await gamer.delay(150)
    }

    // MARK: - Private

    private func drawScreen(gamer: GamerHardware) {
        gamer.clear()
        let bval = gamer.getBrightness()
        for c in 0..<bval { for r in 0..<7 { gamer.display[c][r] = 1 } }
        if gamer.getLEDCompensation() { for c in 0..<8 { gamer.display[c][7] = 1 } }
        gamer.updateDisplay()
    }
}
