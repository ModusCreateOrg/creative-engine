#include "Display.h"

// ODROID
#ifdef __XTENSA__
#include "./OdroidDisplay/OdroidDisplay.h"
static OdroidDisplay child;
Display &gDisplay = child;
#define DISPLAY
#endif

#ifdef __DINGUX__
#include "./LDKDisplay/LDKDisplay.h"
static LDKDisplay child;
Display &gDisplay = child;
#define DISPLAY
#endif

// Networked RGB Matrix
#ifdef __MODUS_TARGET_NETWORK_DISPLAY__
#include "./NetworkDisplay/NetworkDisplay.h"
static NetworkDisplay child;
Display &gDisplay = child;
#define DISPLAY
#endif

// Desktop only
#ifndef DISPLAY
#include "./DesktopDisplay/DesktopDisplay.h"
static DesktopDisplay child;
Display &gDisplay = child;
#endif
