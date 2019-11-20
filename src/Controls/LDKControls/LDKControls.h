#ifdef __DINGUX__

#ifndef MODITE_LDKCONTROLS_H
#define MODITE_LDKCONTROLS_H
#include "Controls.h"
#include <SDL/SDL.h>

class LDKControls : public Controls {
public:
  LDKControls();
  ~LDKControls();

  TBool Poll() final;
};


#endif //MODITE_LDKCONTROLS_H

#endif

