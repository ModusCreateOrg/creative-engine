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
  void Init();
  void SetPalette(TRGB aPalette[], TInt aCount = 256) {
    displayBitmap->SetPalette(aPalette, aCount);
    renderBitmap->SetPalette(aPalette, aCount);
  }
  void SetPalette(BBitmap *aBitmap, TInt aCount = 256) {
    displayBitmap->SetPalette(aBitmap, aCount);
    renderBitmap->SetPalette(aBitmap, aCount);
  }

  void SetColor(TUint8 aIndex, TUint8 aRed, TUint8 aGreen, TUint8 aBlue) {
    displayBitmap->SetColor(aIndex, aRed, aGreen, aBlue);
    renderBitmap->SetColor(aIndex, aRed, aGreen, aBlue);
  }
  void Update();

public:
#ifdef __XTENSA__
  TUint16 color565(TUint8 b, TUint8 r, TUint8 g) {
    // lifted from Display2.cpp
    uint16_t blue = (b & 0b11111000) << 5;
    uint16_t red = (r & 0b11111000);

    uint16_t g2 = (g & 0b00011100) << 11;

    uint16_t g1 = (g & 0b11100000) >> 5;

    uint16_t green = g1 + g2;

    uint16_t final = (uint16_t)(red + green + blue);
    return final;    
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

extern Display gDisplay;
extern TRect gScreenRect;

#endif //DISPLAY_H
