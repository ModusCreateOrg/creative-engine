#ifndef GAME_ENGINE_CONTROLS_H
#define GAME_ENGINE_CONTROLS_H

#include "BTypes.h"

#ifndef __XTENSA__

#define CONTROLLER_SUPPORT
#define CONTROLLER_AXIS_MIN 24000

#include <SDL2/SDL.h>
#endif

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

#define BUTTON_MENU (BUTTON1)
#define BUTTON_SOUND (BUTTON2)
#define BUTTON_SELECT (BUTTON3)
#define BUTTON_START (BUTTON4)
#define BUTTON_ANY (BUTTON1|BUTTON2|BUTTON3|BUTTON4|BUTTONA|BUTTONB)
#define BUTTON_JOY_ANY (BUTTON_ANY|JOYUP|JOYDOWN|JOYRIGHT|JOYLEFT)

class Controls {
public:
  Controls();
  ~Controls();

public:
  void Reset() {
    bKeys = cKeys = dKeys = 0;
  }

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

  void Rumble(TFloat aStrength, TInt aTime);

public:
  TUint16 bKeys, cKeys, dKeys;
#ifdef CONTROLLER_SUPPORT
  SDL_Haptic         *haptic;
  SDL_GameController *ctrl;
#endif
};

extern Controls gControls;

#endif //GAME_ENGINE_CONTROLS_H
