/**
 * The purpose of this class is to isolate or encapsulate hardware display differences between targets.
 *
 * For example, when running/debugging on host, accessing display might be through libsdl2,
 * while on target, direct hardware or I/O might be required to update the screen.
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include "BBitmap.h"

// screen attributes
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_DEPTH 8

#define DISPLAY_WIDTH (SCREEN_WIDTH)
#define DISPLAY_HEIGHT (SCREEN_HEIGHT)

class Display {
public:
  Display();

  virtual ~Display();

public:
  void SetPalette(TRGB aPalette[], TInt aCount = 256) {
    displayBitmap->SetPalette(aPalette, aCount);
    renderBitmap->SetPalette(aPalette, aCount);
  }
  void SetPalette(BBitmap *aBitmap, TInt aCount = 256) {
    displayBitmap->SetPalette(aBitmap, aCount);
    renderBitmap->SetPalette(aBitmap, aCount);
  }
  void Update();

public:
#ifdef __XTENSA__
  TUint16 color565(TUint8 red, TUint8 green, TUint8 blue) {
    // lifted from Display2.cpp
    return ((blue & 0xF8) << 8) | ((red & 0xFC) << 3) | ((green & 0xF8) >> 3);
  }
#else
  TUint16 color565(TUint8 red, TUint8 green, TUint8 blue) {
    // lifted from Display2.cpp
    // maybe we want to return something else for SDL
    return ((blue & 0xF8) << 8) | ((red & 0xFC) << 3) | ((green & 0xF8) >> 3);
  }
#endif

protected:
  // 2 bitmaps for double buffering
  BBitmap *mBitmap1, *mBitmap2;
public:
  BBitmap *displayBitmap, *renderBitmap;
};

extern Display display;

#endif //DISPLAY_H
