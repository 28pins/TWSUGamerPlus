#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "../games/game_interface.h"
#include "../assets/progmem_assets.h"

// ── Launcher state ────────────────────────────────────────────────────────────
#define LAUNCHER_MAX_GAMES 7
static GameDescriptor _games[LAUNCHER_MAX_GAMES];
static byte _numGames = 0;
static byte _gameNumber = 0;
static byte _animFrame = 0;
bool isInLauncher = true;

// Register a game into the launcher list
inline void registerGame(const char* name, void (*reset)(), void (*loop_fn)(),
                         const byte* animFrames, byte numFrames) {
  if (_numGames < LAUNCHER_MAX_GAMES) {
    _games[_numGames].name       = name;
    _games[_numGames].reset      = reset;
    _games[_numGames].loop_fn    = loop_fn;
    _games[_numGames].animFrames = animFrames;
    _games[_numGames].numFrames  = numFrames;
    _numGames++;
  }
}

void launcherSetup() {
  registerGame("SNAKE",  setupSnakeGame,      snakeLoop,   &snake_pgm[0][0],      SNAKE_ANIM_FRAMES);
  registerGame("BRKOUT", startBreakoutReset,  breakoutLoop,&breakout_pgm[0][0],   BREAKOUT_ANIM_FRAMES);
  registerGame("SIMON",  resetSimon,          simonLoop,   &simon_pgm[0][0],      SIMON_ANIM_FRAMES);
  registerGame("FLAPPY", resetFlappyLauncher, flappyLoop,  &flappy_pgm[0][0],     FLAPPY_ANIM_FRAMES);
  registerGame("TETRIS", resetTetris,         tetrisLoop,  &tetris_pgm[0][0],     TETRIS_ANIM_FRAMES);
  registerGame("ALIEN",  resetAlienGame,      alienLoop,   &alienAnim_pgm[0][0],  ALIEN_ANIM_FRAMES);
  registerGame("CONWAY", resetConway,         conwayLoop,  &conwayAnim_pgm[0][0], CONWAY_ANIM_FRAMES);

  // Show startup logo from PROGMEM
  byte buf[8];
  pgm_readimg(startup_pgm[0], buf);
  gamer.printImage(buf);
  delay(100);
}

void showHighScore(byte gameIndex) {
  
}

void launcherLoop() {
  checkSoundToggle();
  if (gamer.isPressed(START)) {
    // Launch selected game; run until START pressed again
    _games[_gameNumber].reset();
    while (!gamer.isPressed(START)) {
      _games[_gameNumber].loop_fn();
    }
    gamer.stopTone();
  } else {
    // Show animation frame from PROGMEM
    if(isInLauncher) {
      byte buf[8];
      pgm_readimg(_games[_gameNumber].animFrames + _animFrame * 8, buf);
      gamer.printImage(buf);
      _animFrame++;
      if (_animFrame >= _games[_gameNumber].numFrames) _animFrame = 0;
    } else {
      showHighScore(_gameNumber);
    }
    if (gamer.isPressed(LEFT)) {
      _gameNumber = (_gameNumber == 0) ? _numGames - 1 : _gameNumber - 1;
      _animFrame = 0;
    }
    if (gamer.isPressed(RIGHT)) {
      _gameNumber = (_gameNumber + 1) % _numGames;
      _animFrame = 0;
    }
    if (gamer.isPressed(UP)) {
      isInLauncher = false;
    } else if (gamer.isPressed(DOWN)) {
      isInLauncher = true;
    }
    delay(300);
  }
}

#endif // LAUNCHER_H
