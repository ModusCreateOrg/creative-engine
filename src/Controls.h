//
// Created by Michael Schwartz on 9/4/18.
//

#ifndef GAME_ENGINE_CONTROLS_H
#define GAME_ENGINE_CONTROLS_H


#include "BTypes.h"

#define BUTTON1 (1<<0)
#define BUTTON2 (1<<1)
#define BUTTON3 (1<<2)
#define BUTTON4 (1<<3)
#define BUTTONA (1<<4)
#define BUTTONB (1<<5)
#define JOYUP (1<<6)
#define JOYDOWN (1<<7)
#define JOYLEFT (1<<8)
#define JOYRIGHT (1<<9)
#define BUTTONQ (1<<10)

class Controls {
public:
  Controls() {
    bKeys = cKeys = dKeys = 0;
  }

public:
  TBool Poll();

public:
  TBool WasPressed(TUint16 bits) {
    if (dKeys & bits) {
      dKeys &= ~bits;
      return ETrue;
    }
    return EFalse;
  }

  TBool IsPressed(TUint16 bits) {
    return (cKeys & bits) ? ETrue : EFalse;
  }

public:
  TUint16 bKeys, cKeys, dKeys;
};

extern Controls controls;

#endif //GAME_ENGINE_CONTROLS_H
