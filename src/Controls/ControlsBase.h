#ifndef CONTROLSBASE_H
#define CONTROLSBASE_H

#include "BTypes.h"

const TUint16 BUTTON1 = TUint16(1<<0);
const TUint16 BUTTON2 = TUint16(1<<1);
const TUint16 BUTTON3 = TUint16(1<<2);
const TUint16 BUTTON4 = TUint16(1<<3);
const TUint16 BUTTONA = TUint16(1<<4);
const TUint16 BUTTONB = TUint16(1<<5);
const TUint16 JOYUP = TUint16(1<<6);
const TUint16 JOYDOWN = TUint16(1<<7);
const TUint16 JOYLEFT = TUint16(1<<8);
const TUint16 JOYRIGHT = TUint16(1<<9);
const TUint16 BUTTONQ = TUint16(1<<10);

const TUint16 BUTTON_MENU = (BUTTON1);
const TUint16 BUTTON_SOUND = (BUTTON2);
const TUint16 BUTTON_SELECT = (BUTTON3);
const TUint16 BUTTON_START = (BUTTON4);
const TUint16 BUTTON_ANY = (BUTTON1|BUTTON2|BUTTON3|BUTTON4|BUTTONA|BUTTONB);
const TUint16 JOY_ANY = (JOYUP|JOYDOWN|JOYLEFT|JOYRIGHT);
const TUint16 BUTTON_JOY_ANY = (BUTTON_ANY|JOY_ANY);

class ControlsBase {
public:
  ControlsBase();
  ~ControlsBase();

public:
  void Reset() {
    bKeys = cKeys = dKeys = 0;
  }


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

  void Rumble(TFloat aStrength, TInt aTime) {}

  virtual TBool Poll() = 0;


public:
  TUint16 bKeys, cKeys, dKeys;

};


#endif //CONTROLSBASE_H
