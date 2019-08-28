#ifdef __DINGUX__

#ifndef MODITE_LDKCONTROLS_H
#define MODITE_LDKCONTROLS_H
#include "ControlsBase.h"
#include <SDL.h>

class LDKControls : public ControlsBase {
public:
  LDKControls();
  ~LDKControls();


  TBool Poll() final;

#ifdef CONTROLLER_SUPPORT
  SDL_Haptic         *haptic;
  SDL_GameController *ctrl;
#endif
};


#endif //MODITE_LDKCONTROLS_H

#endif

