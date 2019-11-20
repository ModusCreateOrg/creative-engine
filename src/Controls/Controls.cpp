#include "Controls.h"


Controls::Controls() {
  Reset();
}

Controls::~Controls() = default;

// ODROID
#ifdef __XTENSA__
#include "./OdroidControls/OdroidControls.h"
static OdroidControls child;
Controls &gControls = child;
#define DISPLAY
#endif

#ifdef __DINGUX__
#include "./LDKControls/LDKControls.h"
static LDKControls child;
Controls &gControls = child;
#define DISPLAY
#endif

// Networked RGB Matrix
#ifdef __MODUS_TARGET_NETWORK_DISPLAY__
#include "./NetworkControls/NetworkControls.h"
static NetworkControls child;
Controls &gControls = child;
#define DISPLAY
#endif

// Desktop only
#ifndef DISPLAY
#include "./DesktopControls/DesktopControls.h"
static DesktopControls child;
Controls &gControls = child;
#endif
