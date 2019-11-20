#ifdef __MODUS_TARGET_DESKTOP_CONTROLS__

#ifndef DESKTOPCONTROLS_H
#define DESKTOPCONTROLS_H


#include "Controls.h"
#include <SDL.h>

#define CONTROLLER_SUPPORT

#define CONTROLLER_AXIS_MIN 24000

class DesktopControls : public Controls {
public:
  DesktopControls();
  ~DesktopControls();

  void Rumble(TFloat aStrength, TInt aTime);

  TBool Poll() final;

#ifdef CONTROLLER_SUPPORT
  SDL_Haptic         *haptic;
  SDL_GameController *ctrl;
#endif
};

#endif // DESKTOPCONTROLS_H

#endif //__MODUS_TARGET_DESKTOP__