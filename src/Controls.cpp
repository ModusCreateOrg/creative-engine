//
// Created by Michael Schwartz on 9/4/18.
//

#include "Controls.h"

#ifndef __XTENSA__

#include "SDL2/SDL.h"

#endif

Controls controls;

TBool Controls::Poll() {
#ifndef __XTENSA__
  SDL_Event e;

  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      return false;
    }
    TUint16 keys = 0;
    if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.scancode) {
        // QUIT button, will never be set on target
        case SDL_SCANCODE_Q:
          keys |= BUTTONQ;
          break;
        case SDL_SCANCODE_1:
          keys |= BUTTON1;
          break;
        case SDL_SCANCODE_2:
          keys |= BUTTON2;
          break;
        case SDL_SCANCODE_3:
          keys |= BUTTON3;
          break;
        case SDL_SCANCODE_4:
          keys |= BUTTON4;
          break;
        case SDL_SCANCODE_RALT:
        case SDL_SCANCODE_LCTRL:
        case SDL_SCANCODE_Z:
          keys |= BUTTONA;
          break;
        case SDL_SCANCODE_X:
        case SDL_SCANCODE_RCTRL:
        case SDL_SCANCODE_LALT:
          keys |= BUTTONB;
          break;
        case SDL_SCANCODE_K:
        case SDL_SCANCODE_UP:
          keys |= JOYUP;
          break;
        case SDL_SCANCODE_J:
        case SDL_SCANCODE_DOWN:
          keys |= JOYDOWN;
          break;
        case SDL_SCANCODE_H:
        case SDL_SCANCODE_LEFT:
          keys |= JOYLEFT;
          break;
        case SDL_SCANCODE_L:
        case SDL_SCANCODE_RIGHT:
          keys |= JOYRIGHT;
          break;
        default:
          break;
      }
    }
    // cKeys are journaled if journaling is on!
    dKeys |= (keys ^ cKeys) & keys;
    cKeys        = keys;
    bKeys        = keys;
//    if (e.type == SDL_MOUSEBUTTONDOWN) {
//      quit = true;
//    }
  }
  return false;
#endif
}
