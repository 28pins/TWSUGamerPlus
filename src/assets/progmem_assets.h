// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

#ifndef PROGMEM_ASSETS_H
#define PROGMEM_ASSETS_H
#include <avr/pgmspace.h>

// ── Animation frame counts ────────────────────────────────────────────────────
#define STARTUP_ANIM_FRAMES   1
#define SNAKE_ANIM_FRAMES     2
#define BREAKOUT_ANIM_FRAMES  2
#define SIMON_ANIM_FRAMES     2
#define FLAPPY_ANIM_FRAMES    2
#define TETRIS_ANIM_FRAMES    2
#define ALIEN_ANIM_FRAMES     2
#define CONWAY_ANIM_FRAMES    2
#define DINO_ANIM_FRAMES      2
#define BRIGHT_ANIM_FRAMES    2

// ── Launcher animation frames in PROGMEM ────────────────────────────────────

static const byte startup_pgm[1][8] PROGMEM = {
  {B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111}
};

// Frame 0: S-curve snake + food dot; Frame 1: food dot off (blinking)
static const byte snake_pgm[2][8] PROGMEM = {
  {B00000000, B00111000, B00001000, B00001110, B00000000, B01000000, B00000000, B00000000},
  {B00000000, B00111000, B00001000, B00001110, B00000000, B00000000, B00000000, B00000000}
};

// Frame 0: full block rows + ball + paddle; Frame 1: one block broken, ball moved
static const byte breakout_pgm[2][8] PROGMEM = {
  {B11111111, B11111111, B00000000, B00010000, B00000000, B00000000, B00000000, B00111000},
  {B11111111, B11011111, B00000000, B00000000, B00010000, B00000000, B00000000, B00111000}
};

// Frame 0: top-left+bottom-right lit; Frame 1: top-right+bottom-left lit
static const byte simon_pgm[2][8] PROGMEM = {
  {B11110000, B11110000, B11110000, B11110000, B00001111, B00001111, B00001111, B00001111},
  {B00001111, B00001111, B00001111, B00001111, B11110000, B11110000, B11110000, B11110000}
};

// Frame 0: bird wing up + pipe; Frame 1: bird wing down + pipe
static const byte flappy_pgm[2][8] PROGMEM = {
  {B00000001, B00000001, B01000000, B01100000, B00000000, B00000000, B00000001, B00000001},
  {B00000001, B00000001, B00000000, B01100000, B01000000, B00000000, B00000001, B00000001}
};

// Frame 0: O-piece at rows 0-1; Frame 1: O-piece dropped to rows 1-2
static const byte tetris_pgm[2][8] PROGMEM = {
  {B00110000, B00110000, B00000000, B00000000, B00000000, B00000000, B11001111, B11111111},
  {B00000000, B00110000, B00110000, B00000000, B00000000, B00000000, B11001111, B11111111}
};

// Frame 0: alien1 sprite (legs down); Frame 1: alien2 sprite (arms spread)
static const byte alienAnim_pgm[2][8] PROGMEM = {
  {B00000000, B00000000, B01111110, B01011010, B01111110, B00100100, B00100100, B01100110},
  {B00000000, B01111110, B01011010, B01111110, B00100100, B01000010, B11000011, B00000000}
};

// Frame 0: glider step 0 (.#. / ..# / ###); Frame 1: glider step 1 (#.# / .## / .#.)
static const byte conwayAnim_pgm[2][8] PROGMEM = {
  {B01000000, B00100000, B11100000, B00000000, B00000000, B00000000, B00000000, B00000000},
  {B10100000, B01100000, B01000000, B00000000, B00000000, B00000000, B00000000, B00000000}
};

// Frame 0: dino standing + cactus; Frame 1: dino jumping over cactus
static const byte dinoAnim_pgm[2][8] PROGMEM = {
  {B00000000, B00000000, B00000000, B00000000, B00000000, B01000100, B01000100, B11111111},
  {B00000000, B00000000, B00000000, B01000000, B01000000, B00000100, B00000100, B11111111}
};

// Frame 0: sun outline (dim); Frame 1: sun filled (bright)
static const byte brightAnim_pgm[2][8] PROGMEM = {
  {B00000000, B01000010, B00100100, B00011000, B00011000, B00100100, B01000010, B00000000},
  {B00000000, B01100110, B00111100, B01111110, B01111110, B00111100, B01100110, B00000000}
};

// ── In-game image assets in PROGMEM ─────────────────────────────────────────

// Breakout win/lose frames
static const byte framesBreakout_pgm[2][8] PROGMEM = {
  {B00000000, B01100110, B01100110, B00000000, B01000010, B00111100, B00000000, B00000000},
  {B00000000, B01100110, B01100110, B00000000, B00000000, B00111100, B01000010, B00000000}
};

// Simon arrow frames: [0]=up [1]=down [2]=left [3]=right
static const byte framesSimon_pgm[4][8] PROGMEM = {
  {B00000000, B00011000, B00111100, B01111110, B00011000, B00011000, B00011000, B00000000}, // up
  {B00000000, B00011000, B00011000, B00011000, B01111110, B00111100, B00011000, B00000000}, // down
  {B00000000, B00010000, B00110000, B01111110, B01111110, B00110000, B00010000, B00000000}, // left
  {B00000000, B00001000, B00001100, B01111110, B01111110, B00001100, B00001000, B00000000}  // right
};

// Simon "GO" graphic
static const byte go_pgm[8] PROGMEM = {
  B00000000, B01101110, B10001010, B10001010, B10001010, B10101010, B01101110, B00100000
};

// Simon "WRONG" (X) graphic
static const byte wrong_pgm[8] PROGMEM = {
  B11000011, B01100110, B00111100, B00011000, B00011000, B00111100, B01100110, B11000011
};

// Simon "RIGHT" (checkmark) graphic
static const byte right_pgm[8] PROGMEM = {
  B00000001, B00000011, B00000111, B00001110, B11011100, B11111000, B01110000, B00100000
};

// ── Numbers for 2-digit score display (3-pixel wide, shift×5 for tens) ──────
static const byte numbers_pgm[10][8] PROGMEM = {
  { B00000111, B00000101, B00000101, B00000101, B00000101, B00000101, B00000101, B00000111 }, // 0
  { B00000100, B00000100, B00000100, B00000100, B00000100, B00000100, B00000100, B00000100 }, // 1
  { B00000111, B00000001, B00000001, B00000111, B00000100, B00000100, B00000100, B00000111 }, // 2
  { B00000111, B00000001, B00000001, B00000011, B00000001, B00000001, B00000001, B00000111 }, // 3
  { B00000101, B00000101, B00000101, B00000111, B00000001, B00000001, B00000001, B00000001 }, // 4
  { B00000111, B00000100, B00000100, B00000111, B00000001, B00000001, B00000001, B00000111 }, // 5
  { B00000111, B00000100, B00000100, B00000111, B00000101, B00000101, B00000101, B00000111 }, // 6
  { B00000111, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001 }, // 7
  { B00000111, B00000101, B00000101, B00000111, B00000101, B00000101, B00000101, B00000111 }, // 8
  { B00000111, B00000101, B00000101, B00000111, B00000001, B00000001, B00000001, B00000111 }, // 9
};

// ── Tetris background melody (Korobeiniki A-theme fragment) in PROGMEM ───────
// Note values for gamer.playTone(): OCR2A register bytes (frequency ≈ 1 MHz / (n+1))
// NOTE_B7=252 (~3937 Hz)  NOTE_C8=238 (~4202 Hz)  NOTE_D8=212 (~4717 Hz)
// NOTE_E8=189 (~5263 Hz)  NOTE_A8=140 (~7042 Hz)
static const byte tetrisMelody_pgm[] PROGMEM = {
  189, 252, 238, 212, 212, 238, 252, 238,  // NOTE_E8 B7 C8 D8 D8 C8 B7 C8
  189, 140, 140, 238, 189, 212, 238, 252   // NOTE_E8 A8 A8 C8 E8 D8 C8 B7
};
#define TETRIS_MELODY_LEN 16

// ── Helpers ──────────────────────────────────────────────────────────────────

// PROGMEM self-check: reads first byte of startup_pgm[0]; expected 0xFF
inline byte progmemSelfCheck() {
  return pgm_read_byte(&startup_pgm[0][0]);
}

// Copy an 8-byte PROGMEM image row into a RAM buffer for gamer.printImage()
inline void pgm_readimg(const byte* pgm_src, byte* dst) {
  for (byte i = 0; i < 8; i++) dst[i] = pgm_read_byte(pgm_src + i);
}

#endif // PROGMEM_ASSETS_H
