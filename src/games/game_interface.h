#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H
#include "Arduino.h"

// Each game must provide a GameDescriptor at registration
struct GameDescriptor {
  const char* name;       // short game name
  void (*reset)();        // called when game is selected
  void (*loop_fn)();      // called each tick while game is active
  const byte* animFrames; // PROGMEM pointer to animation frames (rows of 8 bytes)
  byte numFrames;         // number of animation frames
};

#endif // GAME_INTERFACE_H
