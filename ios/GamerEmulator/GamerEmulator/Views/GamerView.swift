// GamerView.swift — Root emulator view: display + controls + status bar.
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

import SwiftUI

// MARK: - Root shell (creates the shared objects)

struct GamerView: View {

    @StateObject private var launcher = Launcher()

    var body: some View {
        // Pass gamer as a separate @ObservedObject so hardware state changes
        // (display, ledOn, soundEnabled, brightness) trigger redraws.
        GamerBodyView(launcher: launcher, gamer: launcher.gamer)
    }
}

// MARK: - Body view (observes both launcher and gamer)

private struct GamerBodyView: View {

    @ObservedObject var launcher: Launcher
    @ObservedObject var gamer:    GamerHardware

    var body: some View {
        ZStack {
            // Background gradient
            LinearGradient(
                colors: [Color(red: 0.08, green: 0.08, blue: 0.12),
                         Color(red: 0.03, green: 0.03, blue: 0.06)],
                startPoint: .top, endPoint: .bottom
            )
            .ignoresSafeArea()

            VStack(spacing: 16) {
                // ── Top bar ────────────────────────────────────────────────
                HStack {
                    // Game name / mode label
                    VStack(alignment: .leading, spacing: 2) {
                        Text(launcher.games[launcher.currentGameIndex].name)
                            .font(.system(size: 18, weight: .bold, design: .monospaced))
                            .foregroundColor(.white)
                        Text(launcher.isInGame
                             ? "PLAYING"
                             : (launcher.isInLauncher ? "SELECT" : "HI SCORE"))
                            .font(.system(size: 10, weight: .semibold, design: .rounded))
                            .foregroundColor(.orange.opacity(0.8))
                    }
                    Spacer()

                    // Red indicator LED
                    Circle()
                        .fill(gamer.ledOn ? Color.red : Color(red: 0.3, green: 0.05, blue: 0.05))
                        .frame(width: 12, height: 12)
                        .shadow(color: gamer.ledOn ? .red.opacity(0.8) : .clear, radius: 6)
                        .animation(.easeInOut(duration: 0.1), value: gamer.ledOn)

                    // Sound toggle
                    CapTouchButton(gamer: gamer)
                }
                .padding(.horizontal, 16)

                // ── 8×8 LED matrix ─────────────────────────────────────────
                MatrixDisplayView(gamer: gamer, cellSize: 36)
                    .padding(.horizontal, 12)

                // ── Controls ───────────────────────────────────────────────
                HStack(alignment: .center, spacing: 32) {

                    DPadView(gamer: gamer, btnSize: 50)

                    Spacer()

                    VStack(spacing: 12) {
                        // START / EXIT button — does NOT queue a button press;
                        // launch/exit is handled directly in the view model.
                        Button {
                            handleStartPress()
                        } label: {
                            ZStack {
                                Circle()
                                    .fill(Color(red: 0.7, green: 0.1, blue: 0.1))
                                    .frame(width: 58, height: 58)
                                    .shadow(color: .black.opacity(0.5), radius: 4, x: 0, y: 3)
                                Text(launcher.isInGame ? "EXIT" : "START")
                                    .font(.system(size: 13, weight: .bold, design: .rounded))
                                    .foregroundColor(.white)
                            }
                        }
                        .buttonStyle(.plain)

                        // High-score indicator
                        if !launcher.isInGame {
                            let hs = HighScoreStore.getHighScore(launcher.currentGameIndex)
                            Text("BEST: \(hs)")
                                .font(.system(size: 10, weight: .semibold, design: .monospaced))
                                .foregroundColor(.orange.opacity(0.7))
                        }
                    }
                }
                .padding(.horizontal, 24)

                Spacer(minLength: 8)

                // ── Footer ─────────────────────────────────────────────────
                Text("TWSU GAMER EMULATOR")
                    .font(.system(size: 9, weight: .medium, design: .monospaced))
                    .foregroundColor(.white.opacity(0.2))
                    .padding(.bottom, 4)
            }
        }
        .statusBarHidden(true)
    }

    // MARK: - Helpers

    private func handleStartPress() {
        if launcher.isInGame {
            launcher.exitCurrentGame()
        } else {
            launcher.launchCurrentGame()
        }
    }
}

#Preview {
    GamerView()
}

