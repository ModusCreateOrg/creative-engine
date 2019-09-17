#ifndef CONTROLS_H
#define CONTROLS_H



// ODROID
#ifdef __XTENSA__
#include "OdroidControls.h"
class Controls : public OdroidControls {};
#endif

// Networked RGB Matrix
#ifdef __MODUS_TARGET_DIRECT_LINUX_CONTROLS__
#include "DirectLinuxControls.h"
class Controls : public DirectLinuxControls {};
#endif

#ifdef __DINGUX__
#include "LDKControls.h"
class Controls : public LDKControls {};
#endif


// Desktop only
#ifdef __MODUS_TARGET_DESKTOP_CONTROLS__
#include "DesktopControls.h"
class Controls : public DesktopControls {};
#endif






extern Controls gControls;


#endif //CONTROLS_H
