// ButtonsView.swift — Physical button layout for the Gamer emulator.
// Replicates the TWSU Gamer Kit's five-button layout plus a cap-touch pad.
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

import SwiftUI

// MARK: - Single hardware button

struct HardwareButton: View {

    let label:   String
    let button:  GamerButton
    let gamer:   GamerHardware
    var size:    CGFloat = 52
    var color:   Color   = Color(red: 0.2, green: 0.2, blue: 0.25)
    var fgColor: Color   = .white

    @State private var isDown = false

    var body: some View {
        ZStack {
            Circle()
                .fill(isDown ? Color.orange.opacity(0.7) : color)
                .frame(width: size, height: size)
                .shadow(color: .black.opacity(0.5), radius: 4, x: 0, y: 3)
            Text(label)
                .font(.system(size: size * 0.32, weight: .bold, design: .rounded))
                .foregroundColor(fgColor)
        }
        .gesture(
            DragGesture(minimumDistance: 0)
                .onChanged { _ in
                    if !isDown {
                        isDown = true
                        gamer.buttonDown(button)
                    }
                }
                .onEnded { _ in
                    isDown = false
                    gamer.buttonUp(button)
                }
        )
    }
}

// MARK: - D-pad cluster (UP / DOWN / LEFT / RIGHT)

struct DPadView: View {

    let gamer: GamerHardware
    var btnSize: CGFloat = 52

    var body: some View {
        VStack(spacing: 4) {
            HardwareButton(label: "▲", button: .up,    gamer: gamer, size: btnSize)
            HStack(spacing: 4) {
                HardwareButton(label: "◀", button: .left,  gamer: gamer, size: btnSize)
                // Centre dead-zone
                Circle()
                    .fill(Color(red: 0.15, green: 0.15, blue: 0.2))
                    .frame(width: btnSize, height: btnSize)
                HardwareButton(label: "▶", button: .right, gamer: gamer, size: btnSize)
            }
            HardwareButton(label: "▼", button: .down,  gamer: gamer, size: btnSize)
        }
    }
}

// MARK: - Capacitive-touch / sound toggle pad

struct CapTouchButton: View {

    @ObservedObject var gamer: GamerHardware
    @State private var tapped = false

    var body: some View {
        Button {
            tapped = true
            gamer.capTouchTap()
            DispatchQueue.main.asyncAfter(deadline: .now() + 0.15) { tapped = false }
        } label: {
            ZStack {
                Capsule()
                    .fill(tapped ? Color.orange.opacity(0.6) : Color(red: 0.2, green: 0.2, blue: 0.25))
                    .frame(width: 90, height: 34)
                    .shadow(color: .black.opacity(0.4), radius: 3, x: 0, y: 2)
                HStack(spacing: 4) {
                    Image(systemName: gamer.soundEnabled ? "speaker.wave.2.fill" : "speaker.slash.fill")
                        .font(.system(size: 12, weight: .semibold))
                        .foregroundColor(gamer.soundEnabled ? .orange : .gray)
                    Text("SOUND")
                        .font(.system(size: 11, weight: .bold, design: .rounded))
                        .foregroundColor(.white.opacity(0.7))
                }
            }
        }
        .buttonStyle(.plain)
    }
}
