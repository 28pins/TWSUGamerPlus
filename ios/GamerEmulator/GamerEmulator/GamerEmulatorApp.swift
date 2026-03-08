// GamerEmulatorApp.swift — SwiftUI application entry point.
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

import SwiftUI

@main
struct GamerEmulatorApp: App {

    init() {
        // First-launch high-score initialisation (mirrors Arduino EEPROM init)
        let key = "highscores_initialised"
        if !UserDefaults.standard.bool(forKey: key) {
            HighScoreStore.resetAll()
            UserDefaults.standard.set(true, forKey: key)
        }
    }

    var body: some Scene {
        WindowGroup {
            ContentView()
        }
    }
}
