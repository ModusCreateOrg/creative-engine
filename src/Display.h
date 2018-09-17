/**
 * The purpose of this class is to isolate or encapsulate hardware display differences between targets.
 *
 * For example, when running/debugging on host, accessing display might be through libsdl2,
 * while on target, direct hardware or I/O might be required to update the screen.
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include "GameEngine.h"

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

protected:
  // 2 bitmaps for double buffering
  BBitmap *mBitmap1, *mBitmap2;
public:
  BBitmap *displayBitmap, *renderBitmap;
};

extern Display display;

#endif //DISPLAY_H
