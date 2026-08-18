// Launcher.swift — Game selection menu ported from src/launcher/launcher.h
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

import Foundation
import Combine

/// Manages the game list, animation cycles, and launching/exiting games.
@MainActor
final class Launcher: ObservableObject {

    // MARK: - Published state

    @Published var currentGameIndex: Int = 0
    @Published var isInLauncher:     Bool = true   // false = showing high scores
    @Published var isInGame:         Bool = false
    @Published var animFrame:        Int  = 0

    // MARK: - Hardware + games

    let gamer = GamerHardware()

    private(set) var games: [any Game]
    private let conwayGame: ConwayGame    // kept for preview access

    private var gameTask:     Task<Void, Never>? = nil
    private var launcherTask: Task<Void, Never>? = nil

    // MARK: - Init

    init() {
        let snake    = SnakeGame()
        let breakout = BreakoutGame()
        let simon    = SimonGame()
        let flappy   = FlappyGame()
        let tetris   = TetrisGame()
        let alien    = AlienGame()
        let conway   = ConwayGame()
        let dino     = DinoGame()
        let bright   = BrightnessGame()

        conwayGame = conway
        games = [snake, breakout, simon, flappy, tetris, alien, conway, dino, bright]

        conwayGame.randomize()
        // Show startup splash (all LEDs on briefly)
        Task { @MainActor in
            self.gamer.allOn()
            await self.gamer.delay(100)
            self.gamer.clear()
            self.startLauncherLoop()
        }
    }

    // MARK: - Launcher loop

    private func startLauncherLoop() {
        launcherTask?.cancel()
        launcherTask = Task { @MainActor [weak self] in
            guard let self else { return }
            while !Task.isCancelled && !self.isInGame {
                await self.launcherTick()
            }
        }
    }

    private func launcherTick() async {
        guard !isInGame else { return }

        gamer.updateLEDFlash()

        // Sound toggle (cap touch emulated via UI button)
        // Navigation
        if gamer.isPressed(.start) {
            launchCurrentGame()
            return
        } else if gamer.isPressed(.left) {
            gamer.clear()
            currentGameIndex = (currentGameIndex == 0) ? games.count - 1 : currentGameIndex - 1
            animFrame = 0
            if currentGameIndex == 6 { conwayGame.randomize() }
        } else if gamer.isPressed(.right) {
            gamer.clear()
            currentGameIndex = (currentGameIndex + 1) % games.count
            animFrame = 0
            if currentGameIndex == 6 { conwayGame.randomize() }
        } else if gamer.isPressed(.up) {
            isInLauncher = false
        } else if gamer.isPressed(.down) {
            isInLauncher = true
        }

        // Render
        if isInLauncher {
            if gamer.ledOn {
                gamer.setLED(false)
            }
            if currentGameIndex == 6 {
                // Live Conway preview in centre 4×4
                let changed = conwayGame.stepSmall()
                if !changed { conwayGame.randomize() }
                let curr = conwayGame.getCurr()
                for x in 0..<8 { for y in 0..<8 { gamer.display[x][y] = 0 } }
                for x in 2..<6 { for y in 2..<6 { gamer.display[x][y] = (curr[y] >> x) & 1 } }
                gamer.updateDisplay()
            } else {
                let frames = games[currentGameIndex].animFrames
                let frame  = frames[animFrame % frames.count]
                gamer.printImage(frame)
                animFrame = (animFrame + 1) % frames.count
            }
        } else {
            // High score view
            let hs = HighScoreStore.getHighScore(currentGameIndex)
            gamer.showScore(hs / 10, hs % 10)
            if !gamer.ledOn { gamer.startLEDFlash() }
        }

        await gamer.delay(300)
    }

    // MARK: - Game launch / exit

    func launchCurrentGame() {
        launcherTask?.cancel()
        launcherTask = nil

        let game          = games[currentGameIndex]
        let fromHS        = !isInLauncher
        let startingScore = UInt8(min(255, HighScoreStore.getHighScore(currentGameIndex)))

        game.reset(fromHighScore: fromHS, startingScore: startingScore)

        // Clear any lingering button state so it doesn't bleed into the game
        gamer.clearAllButtons()

        isInGame = true

        gameTask = Task { @MainActor [weak self] in
            guard let self else { return }
            while !Task.isCancelled {
                await game.loop(gamer: self.gamer)
            }
        }
    }

    func exitCurrentGame() {
        gameTask?.cancel()
        gameTask = nil
        gamer.stopTone()
        gamer.clear()
        gamer.clearAllButtons()
        isInGame = false
        // Return to launcher loop
        startLauncherLoop()
    }
}
