#ifndef SIMON_H
#define SIMON_H

// ── Simon game state ──────────────────────────────────────────────────────────
byte x=0;
int delayMils = 300;
volatile byte sequence[30];

void resetSimon() {
  gamer.clear();
  delay(100);
  for(byte b=0;b<x;b++) sequence[b]=0;
  x=0;
  delayMils = 300;
}

void simonLoop() {
  checkSoundToggle();
  updateLEDFlash();
  static const byte simonNotes[] PROGMEM = {NOTE_E8, NOTE_C8, NOTE_G8, NOTE_D8};
  sequence[x]=random(0, SIMON_NUM_DIRECTIONS);
  if(x>0) {
    for(byte p=3;p>0;p--) {
      showScore(0,p);
      delay(delayMils);
    }
    byte gbuf[8]; pgm_readimg(go_pgm, gbuf);
    gamer.printImage(gbuf);
    delay(delayMils);
    for(byte i=0;i<x;i++) {
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
    boolean success = true;
    for(byte count=0;count<x;count++) {
      if(gamer.isHeld(START)) return;
      byte key = 4;
      while(key==4) {
        if(gamer.isHeld(START)) return;
        if(gamer.isHeld(UP)) key=0;
        if(gamer.isHeld(DOWN)) key=1;
        if(gamer.isHeld(LEFT)) key=2;
#if SIMON_RIGHT_ARROW_ENABLED
        if(gamer.isHeld(RIGHT)) key=3;
#endif
        while(gamer.isHeld(RIGHT) || gamer.isHeld(LEFT) || gamer.isHeld(UP) || gamer.isHeld(DOWN)) {}
      }
      startLEDFlash();
      byte kbuf[8]; pgm_readimg(framesSimon_pgm[key], kbuf);
      gamer.printImage(kbuf);
      if(key!=sequence[count]) {
        success = false;
        break;
      }
    }
    delayMils-=(delayMils/40);
    delay(delayMils);
    if(success) {
      x++;
      playWinTune();
      byte rbuf[8]; pgm_readimg(right_pgm, rbuf);
      gamer.printImage(rbuf);
    }
    else {
      playLossTune();
      byte wbuf[8]; pgm_readimg(wrong_pgm, wbuf);
      gamer.printImage(wbuf);
      delay(400);
      showScore((x-1)/10,(x-1)%10);
      delay(400);
      resetSimon();
    }
  } else {
    x++;
  }
  delay(400);
}

#endif // SIMON_H
