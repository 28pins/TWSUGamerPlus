// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

#ifndef SIMON_H
#define SIMON_H

// ── Simon game state ──────────────────────────────────────────────────────────
byte simonStep = 0;
uint16_t delayMils = 300;
byte sequence[SIMON_MAX_SEQUENCE];

void resetSimon() {
  gamer.clear();
  delay(100);
  if (startFromHighScore && startingHighScore > 0) {
    // Start with half the high score (capped at max sequence length)
    simonStep = min(7, startingHighScore / 3);
    // Pre-populate the sequence with random values
    for(byte b=0;b<simonStep;b++) sequence[b] = random(0, SIMON_NUM_DIRECTIONS);
    // Calculate delay based on progression (decreases by ~2.5% each round)
    // Starting at 300ms, after n steps: 300 * (0.975^n)
    // Approximate with: 300 - simonStep * 4, minimum 20ms
    delayMils = max(20, 300 - simonStep * 4);
  } else {
    simonStep=0;
    delayMils = 300;
  }
}

void simonLoop() {
  checkSoundToggle();
  updateLEDFlash();
  static const byte simonNotes[] PROGMEM = {NOTE_E8, NOTE_C8, NOTE_G8, NOTE_D8};
  sequence[simonStep]=random(0, SIMON_NUM_DIRECTIONS);
  if(simonStep>0) {
    for(byte p=3;p>0;p--) {
      showScore(0,p);
      delay(delayMils);
    }
    byte gbuf[8]; pgm_readimg(go_pgm, gbuf);
    gamer.printImage(gbuf);
    delay(delayMils);
    for(byte i=0;i<simonStep;i++) {
      if(gamer.isHeld(START)) return;
      if (soundEnabled) gamer.playTone(pgm_read_byte(&simonNotes[sequence[i]]));
      byte fbuf[8]; pgm_readimg(framesSimon_pgm[sequence[i]], fbuf);
      gamer.printImage(fbuf);
      startLEDFlash();
      delay(delayMils);
      if (soundEnabled) gamer.stopTone();
      gamer.clear();
      delay(delayMils);
    }
    gamer.clear();
    bool success = true;
    for(byte count=0;count<simonStep;count++) {
      if(gamer.isHeld(START)) return;
      byte key = 4;
      while(key==4) {
        if(gamer.isHeld(START)) return;
        if(gamer.isHeld(UP)) key=0;
        if(gamer.isHeld(DOWN)) key=1;
        if(gamer.isHeld(LEFT)) key=2;
        if(gamer.isHeld(RIGHT)) key=3;
      }
      startLEDFlash();
      byte kbuf[8]; pgm_readimg(framesSimon_pgm[key], kbuf);
      gamer.printImage(kbuf);
      if(key!=sequence[count]) {
        success = false;
        break;
      }
      while(gamer.isHeld(UP) || gamer.isHeld(DOWN) || gamer.isHeld(LEFT) || gamer.isHeld(RIGHT)) {
        if(gamer.isHeld(START)) return;
        delay(10);
      }
    }
    delayMils-=(delayMils/40);
    delay(delayMils);
    if(success) {
      simonStep++;
      score = simonStep;
      playWinTune();
      byte rbuf[8]; pgm_readimg(right_pgm, rbuf);
      gamer.printImage(rbuf);
    }
    else {
      score = simonStep;
      playLossTune();
      saveHighScore(score, 2);
      byte wbuf[8]; pgm_readimg(wrong_pgm, wbuf);
      gamer.printImage(wbuf);
      delay(400);
      showScore((simonStep-1)/10,(simonStep-1)%10);
      delay(400);
      resetSimon();
    }
  } else {
    simonStep++;
    if(simonStep>28) {
      score = simonStep;
      simonStep=28;
      playWinTune();
    }
  }
  delay(400);
}

#endif // SIMON_H
