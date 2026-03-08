// GameProtocol.swift — Base protocol for all playable games.
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

import Foundation

/// Every game implements this protocol so the launcher can manage them uniformly.
@MainActor
protocol Game: AnyObject {
    /// Short display name shown in the launcher menu.
    var name: String { get }

    /// Launcher animation frames (each element is an 8-byte image row).
    var animFrames: [[UInt8]] { get }

    /// EEPROM/UserDefaults game index for high-score storage.
    var gameIndex: Int { get }

    /// Reset game state.  Called once before entering the game loop.
    /// - Parameters:
    ///   - fromHighScore: true when launched from the high-score menu (handicap mode).
    ///   - startingScore: the stored high score to seed the handicap.
    func reset(fromHighScore: Bool, startingScore: UInt8)

    /// One iteration of the game loop.  May suspend with `await gamer.delay()`.
    func loop(gamer: GamerHardware) async
}
