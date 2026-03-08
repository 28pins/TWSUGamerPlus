// MatrixDisplayView.swift — 8×8 LED matrix display emulation.
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

import SwiftUI

/// Renders the 8×8 LED matrix as a grid of rounded squares.
/// LED colour: amber (#FFA500) when on, dark charcoal when off.
struct MatrixDisplayView: View {

    @ObservedObject var gamer: GamerHardware

    /// Size of each LED cell in points (including gap).
    var cellSize: CGFloat = 38

    private let gap:         CGFloat = 3
    private let ledOnColor   = Color(red: 1.0, green: 0.65, blue: 0.0)    // amber
    private let ledOffColor  = Color(red: 0.12, green: 0.12, blue: 0.12)  // near-black
    private let bgColor      = Color(red: 0.05, green: 0.05, blue: 0.05)

    var body: some View {
        let totalSize = cellSize * 8 + gap * 7

        ZStack {
            bgColor
                .cornerRadius(8)

            VStack(spacing: gap) {
                ForEach(0..<8, id: \.self) { row in
                    HStack(spacing: gap) {
                        ForEach(0..<8, id: \.self) { col in
                            let pixel = gamer.display[col][row]
                            let brightness = CGFloat(gamer.brightness) / 8.0
                            RoundedRectangle(cornerRadius: 4)
                                .fill(pixel == 1
                                      ? ledOnColor.opacity(Double(brightness))
                                      : ledOffColor)
                                .frame(width: cellSize, height: cellSize)
                        }
                    }
                }
            }
            .padding(6)
        }
        .frame(width: totalSize + 12, height: totalSize + 12)
    }
}

#Preview {
    let g = GamerHardware()
    // Draw a test pattern
    g.display[0][0] = 1; g.display[2][2] = 1; g.display[4][4] = 1; g.display[6][6] = 1
    return MatrixDisplayView(gamer: g)
        .padding()
        .background(Color.black)
}
