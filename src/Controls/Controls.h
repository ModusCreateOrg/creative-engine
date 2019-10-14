#ifndef CONTROLS_H
#define CONTROLS_H

// ODROID
#ifdef __XTENSA__
#include "OdroidControls.h"
class Controls : public OdroidControls {};
#define CONTROLS
#endif

// Networked RGB Matrix
#ifdef __MODUS_TARGET_DIRECT_LINUX_CONTROLS__
#include "DirectLinuxControls.h"
class Controls : public DirectLinuxControls {};
#define CONTROLS
#endif

#ifdef __DINGUX__
#include "LDKControls.h"
class Controls : public LDKControls {};
#define CONTROLS
#endif

// Desktop only
#ifndef CONTROLS
#include "DesktopControls/DesktopControls.h"
class Controls : public DesktopControls {};
#endif

extern Controls gControls;

#endif //CONTROLS_H
