/**
 * The purpose of this class is to isolate or encapsulate hardware display differences between targets.
 *
 * For example, when running/debugging on host, accessing display might be through libsdl2,
 * while on target, direct hardware or I/O might be required to update the screen.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "DisplayDefines.h"
#include "DisplayBase.h"
#include "BBase.h"
#include "BBitmap.h"

// ODROID
#ifdef __XTENSA__
#include "OdroidDisplay.h"
class Display : public OdroidDisplay {};
#define DISPLAY
#endif

#ifdef __DINGUX__
#include "LDKDisplay.h"
class Display : public LDKDisplay {};
#define DISPLAY
#endif


// Networked RGB Matrix
#ifdef __MODUS_TARGET_NETWORK_DISPLAY__
#include "NetworkDisplay.h"
class Display : public NetworkDisplay {};
#define DISPLAY
#endif


// Desktop only
#ifndef DISPLAY
#include "./DesktopDisplay/DesktopDisplay.h"
class Display : public DesktopDisplay {};
#endif


extern Display gDisplay;
extern TRect gScreenRect;

#endif //DISPLAY_H
