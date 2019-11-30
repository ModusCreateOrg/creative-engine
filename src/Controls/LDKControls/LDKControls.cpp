#ifdef __DINGUX__

#include "LDKControls.h"



#define LDK_A_BUTTON 29
#define LDK_B_BUTTON 56
#define LDK_X_BUTTON 57
#define LDK_Y_BUTTON 42

#define LDK_SELECT_BUTTON 1
#define LDK_START_BUTTON  28
#define LDK_L_BUTTON      15
#define LDK_R_BUTTON      14
#define LDK_UP_BUTTON 103
#define LDK_DOWN_BUTTON 108
#define LDK_RIGHT_BUTTON 106
#define LDK_LEFT_BUTTON 105

#define LDK_MUTE_BUTTON 107
#define LDK_SCREEN_BUTTON 4


LDKControls::LDKControls() : Controls() {}

LDKControls::~LDKControls() {
}

int pollNum = 0;
TBool LDKControls::Poll()  {
  SDL_Event e;


  while (SDL_PollEvent(&e)) {
    if (pollNum > 100) {
      printf("POLL %i\n", pollNum);
      fflush(stdout);
      pollNum = 0;
    }

    TUint16 keys = bKeys;

    if (e.type == SDL_QUIT) {
      keys |= BUTTONQ;
    }



    // Keyboard
    if (e.type == SDL_KEYDOWN) {
      printf("KEYDOWN %i\n", e.key.keysym.scancode);
      fflush(stdout);

      switch (e.key.keysym.scancode) {
        // QUIT button, will never be set on target
        case LDK_R_BUTTON:// Not mapped on Odroid
          keys |= BUTTONQ;
          break;
        case LDK_L_BUTTON: // MENU on ODROID
          keys |= BUTTON1;
          break;
        case LDK_X_BUTTON: // MUTE on / off on ODROID
          keys |= BUTTON2;
          break;
        case LDK_SELECT_BUTTON: // SELECT on ODROID
          keys |= BUTTON3;
          break;
        case LDK_START_BUTTON: // START on oDROID
          keys |= BUTTON4;
          break;
        case LDK_A_BUTTON:
          keys |= BUTTONA;
          break;
        case LDK_B_BUTTON:
          keys |= BUTTONB;
          break;
        case LDK_UP_BUTTON:
          keys |= JOYUP;
          break;
        case LDK_DOWN_BUTTON:
          keys |= JOYDOWN;
          break;
        case LDK_LEFT_BUTTON:
          keys |= JOYLEFT;
          break;
        case LDK_RIGHT_BUTTON:
          keys |= JOYRIGHT;
          break;
        default:
          break;
      }
    }

    if (e.type == SDL_KEYUP) {
      switch (e.key.keysym.scancode) {
        // QUIT button, will never be set on target
        case LDK_R_BUTTON:// Not mapped on Odroid
          keys &= ~BUTTONQ;
          break;
        case LDK_L_BUTTON: // MENU on ODROID
          keys &= ~BUTTON1;
          break;
        case LDK_X_BUTTON: // MUTE on / off on ODROID
          keys &= ~BUTTON2;
          break;
        case LDK_SELECT_BUTTON: // SELECT on ODROID
          keys &= ~BUTTON3;
          break;
        case LDK_START_BUTTON: // START on oDROID
          keys &= ~BUTTON4;
          break;
        case LDK_A_BUTTON:
          keys &= ~BUTTONA;
          break;
        case LDK_B_BUTTON:
          keys &= ~BUTTONB;
          break;
        case LDK_UP_BUTTON:
          keys &= ~JOYUP;
          break;
        case LDK_DOWN_BUTTON:
          keys &= ~JOYDOWN;
          break;
        case LDK_LEFT_BUTTON:
          keys &= ~JOYLEFT;
          break;
        case LDK_RIGHT_BUTTON:
          keys &= ~JOYRIGHT;
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
}

#endif