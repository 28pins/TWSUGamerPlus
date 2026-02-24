#ifndef ALIEN_H
#define ALIEN_H

// ── Alien (Space Invaders) game state ────────────────────────────────────────
byte sInvRows[3];
int8_t sInvBaseX;
int8_t sInvPlayerY;
int8_t sInvBulletX;
int8_t sInvBulletY;
byte sInvScore;
byte sInvMarchTick;

inline bool sInvAlive(byte row, byte col) { return (sInvRows[row] >> col) & 1; }
inline void sInvKill(byte row, byte col)  { sInvRows[row] &= ~(1 << col); }

byte sInvCount() {
  byte n = 0;
  for (byte r = 0; r < 3; r++)
    for (byte c = 0; c < 3; c++)
      if (sInvAlive(r, c)) n++;
  return n;
}

void resetAlienGame() {
  sInvRows[0] = 0x07;
  sInvRows[1] = 0x07;
  sInvRows[2] = 0x07;
  sInvBaseX   = 5;
  sInvPlayerY = 3;
  sInvBulletX = -1;
  sInvScore   = 0;
  sInvMarchTick = 0;
}

void alienLoop() {
  checkSoundToggle();
  updateLEDFlash();
  if (soundEnabled) gamer.stopTone();

  for (int cx = 0; cx < 8; cx++)
    for (int cy = 0; cy < 8; cy++)
      gamer.display[cx][cy] = 0;

  // ── Bullet movement ──────────────────────────────────────────────────────
  if (sInvBulletX >= 0) {
    sInvBulletX++;
    if (sInvBulletX >= 8) {
      sInvBulletX = -1;
    } else {
      for (byte r = 0; r < 3; r++) {
        byte ay = 1 + r * 2;
        if (sInvBulletY == ay) {
          for (byte c = 0; c < 3; c++) {
            if (sInvAlive(r, c) && (sInvBaseX + (int8_t)c) == sInvBulletX) {
              sInvKill(r, c);
              sInvBulletX = -1;
              sInvScore++;
              if (soundEnabled) gamer.playTone(NOTE_A8);
              startLEDFlash();
              break;
            }
          }
        }
        if (sInvBulletX < 0) break;
      }
    }
  }

  // ── Alien march ──────────────────────────────────────────────────────────
  byte marchRate = (sInvScore < 12) ? (20 - sInvScore) : 8;
  sInvMarchTick++;
  if (sInvMarchTick >= marchRate) {
    sInvMarchTick = 0;
    sInvBaseX--;
    if (sInvBaseX < 0) {
      for (byte b = 0; b < 4; b++) {
        for (int cx = 0; cx < 8; cx++) for (int cy = 0; cy < 8; cy++) gamer.display[cx][cy] = 0;
        gamer.updateDisplay(); delay(120);
        gamer.display[0][(byte)sInvPlayerY] = 1;
        gamer.updateDisplay(); delay(120);
      }
      playLossTune();
      showScore(sInvScore / 10, sInvScore % 10);
      delay(1500);
      resetAlienGame();
      return;
    }
  }

  // ── All aliens killed – next wave ─────────────────────────────────────
  if (sInvCount() == 0) {
    playWinTune();
    sInvRows[0] = sInvRows[1] = sInvRows[2] = 0x07;
    sInvBaseX = 5;
    sInvBulletX = -1;
  }

  // ── Player input ─────────────────────────────────────────────────────────
  if (gamer.isHeld(UP)   && sInvPlayerY > 0) sInvPlayerY--;
  if (gamer.isHeld(DOWN) && sInvPlayerY < 7) sInvPlayerY++;
  if (gamer.isPressed(RIGHT) && sInvBulletX < 0) {
    sInvBulletX = 1;
    sInvBulletY = sInvPlayerY;
    if (soundEnabled) gamer.playTone(NOTE_B7);
    startLEDFlash();
  }

  // ── Draw ─────────────────────────────────────────────────────────────────
  gamer.display[0][(byte)sInvPlayerY] = 1;
  if (sInvBulletX > 0)
    gamer.display[(byte)sInvBulletX][(byte)sInvBulletY] = 1;
  for (byte r = 0; r < 3; r++) {
    byte ay = 1 + r * 2;
    for (byte c = 0; c < 3; c++) {
      if (sInvAlive(r, c)) {
        int8_t ax = sInvBaseX + (int8_t)c;
        if (ax >= 0 && ax < 8) gamer.display[(byte)ax][ay] = 1;
      }
    }
  }
  gamer.updateDisplay();
  delay(80);
}

#endif // ALIEN_H
