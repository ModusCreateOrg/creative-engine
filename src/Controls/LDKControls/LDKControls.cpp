#ifdef __DINGUX__

#include "LDKControls.h"



#define LDK_A_BUTTON 306
#define LDK_B_BUTTON 308
#define LDK_X_BUTTON 32
#define LDK_Y_BUTTON 304

#define LDK_SELECT_BUTTON 999
#define LDK_START_BUTTON  SLDK_RETURN
#define LDK_L_BUTTON      9
#define LDK_R_BUTTON      8




LDKControls::LDKControls() : ControlsBase() {

}

LDKControls::~LDKControls() {
}

TBool LDKControls::Poll()  {
//  SDL_Event e;
//
//  while (SDL_PollEvent(&e)) {
//    TUint16 keys = bKeys;
//    if (e.type == SDL_QUIT) {
//      keys |= BUTTONQ;
//    }
//
//
//    // Keyboard
//    if (e.type == SDL_KEYDOWN) {
//      switch (e.key.keysym.sym) {
//        // QUIT button, will never be set on target
//        case SDL_SCANCODE_Q:
//          keys |= BUTTONQ;
//          break;
//        case SDL_SCANCODE_1:
//          keys |= BUTTON1;
//          break;
//        case SDL_SCANCODE_2:
//          keys |= BUTTON2;
//          break;
////        case SDL_SCANCODE_3:
////          keys |= BUTTON3;
////          break;
//        case SDL_SCANCODE_4:
//          keys |= BUTTON4;
//          break;
//        case SDL_SCANCODE_LALT:
//        case SDL_SCANCODE_X:
//        case SDL_SCANCODE_F:
//          keys |= BUTTONA;
//          break;
////        case SDL_SCANCODE_SPACE:
//        case SDL_SCANCODE_LCTRL:
//        case SDL_SCANCODE_Z:
//        case SDL_SCANCODE_D:
//          keys |= BUTTONB;
//          break;
//        case SDL_SCANCODE_K:
//        case SDL_SCANCODE_UP:
//          keys |= JOYUP;
//          break;
//        case SDL_SCANCODE_J:
//        case SDL_SCANCODE_DOWN:
//          keys |= JOYDOWN;
//          break;
//        case SDL_SCANCODE_H:
//        case SDL_SCANCODE_LEFT:
//          keys |= JOYLEFT;
//          break;
//        case SDL_SCANCODE_L:
//        case SDL_SCANCODE_RIGHT:
//          keys |= JOYRIGHT;
//          break;
//        default:
//          break;
//      }
//    }
//    if (e.type == SDL_KEYUP) {
//      switch (e.key.keysym.scancode) {
//        // QUIT button, will never be set on target
//        case SDL_SCANCODE_Q:
//          keys &= ~BUTTONQ;
//          break;
//        case SDL_SCANCODE_1:
//          keys &= ~BUTTON1;
//          break;
//        case SDL_SCANCODE_2:
//          keys &= ~BUTTON2;
//          break;
//        case SDL_SCANCODE_3:
//          keys &= ~BUTTON3;
//          break;
//        case SDL_SCANCODE_4:
//        case SDL_SCANCODE_RETURN:
//          keys &= ~BUTTON4;
//          break;
//        case SDL_SCANCODE_LALT:
//        case SDL_SCANCODE_X:
//        case SDL_SCANCODE_F:
//          keys &= ~BUTTONA;
//          break;
////        case SDL_SCANCODE_SPACE:
//        case SDL_SCANCODE_LCTRL:
//        case SDL_SCANCODE_Z:
//        case SDL_SCANCODE_D:
//          keys &= ~BUTTONB;
//          break;
//        case SDL_SCANCODE_K:
//        case SDL_SCANCODE_UP:
//          keys &= ~JOYUP;
//          break;
//        case SDL_SCANCODE_J:
//        case SDL_SCANCODE_DOWN:
//          keys &= ~JOYDOWN;
//          break;
//        case SDL_SCANCODE_H:
//        case SDL_SCANCODE_LEFT:
//          keys &= ~JOYLEFT;
//          break;
//        case SDL_SCANCODE_L:
//        case SDL_SCANCODE_RIGHT:
//          keys &= ~JOYRIGHT;
//          break;
//        default:
//          break;
//      }
//    }
//    // cKeys are journaled if journaling is on!
//    dKeys |= (keys ^ cKeys) & keys;
//    cKeys        = keys;
//    bKeys        = keys;
////    if (e.type == SDL_MOUSEBUTTONDOWN) {
////      quit = true;
////    }
//  }
  return false;
}

#endif