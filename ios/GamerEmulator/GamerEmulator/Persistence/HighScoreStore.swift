// HighScoreStore.swift — UserDefaults-backed high-score persistence.
// Mirrors the EEPROM layout from src/persistence/highscore.h.
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

import Foundation

/// One high-score slot per game index (0-8 matching launcher order).
struct HighScoreStore {

    private static let keyPrefix = "highscore_"

    static func getHighScore(_ gameIndex: Int) -> Int {
        let key = keyPrefix + "\(gameIndex)"
        return UserDefaults.standard.integer(forKey: key)
    }

    /// Saves `score` only when it exceeds the stored high score.
    static func saveHighScore(_ score: Int, _ gameIndex: Int) {
        let clamped = min(score, 255)
        let current = getHighScore(gameIndex)
        if clamped > current {
            UserDefaults.standard.set(clamped, forKey: keyPrefix + "\(gameIndex)")
        }
    }

    /// Wipes all high scores (called on first launch, mirroring EEPROM init).
    static func resetAll() {
        for i in 0..<9 {
            UserDefaults.standard.removeObject(forKey: keyPrefix + "\(i)")
        }
    }
}
