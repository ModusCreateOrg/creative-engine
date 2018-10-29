//
// Created by Michael Schwartz on 9/27/18.
//

#ifndef GENUS_BFONT_H
#define GENUS_BFONT_H

#include "BTypes.h"

class BBitmap;

// Font sizes
static const TInt16 FONT_8x8   = 0; // bitmap contains individual images, each 8x8, on a grid
static const TInt16 FONT_16x16 = 1; // 16x16 on a grid
static const TInt16 FONT_32x32 = 2; // etc.
static const TInt16 FONT_64x64 = 3;
static const TInt16 FONT_8x16  = 4;
static const TInt16 FONT_16x8  = 5;
static const TInt16 FONT_8x32  = 6;
static const TInt16 FONT_32x8  = 7;
static const TInt16 FONT_8x64  = 8;
static const TInt16 FONT_64x8  = 9;
static const TInt16 FONT_16x32 = 10;
static const TInt16 FONT_32x16 = 11;
static const TInt16 FONT_16x64 = 12;
static const TInt16 FONT_64x16 = 13;
static const TInt16 FONT_32x64 = 14;
static const TInt16 FONT_64x32 = 15;
static const TInt16 FONT_32x40 = 16;

/**
 * BFont references the slots in a bitmap that contain fonts.
 */
class BFont {
public:
  BFont(BBitmap *b, TInt16 fontType);
  ~BFont();
public:
  TRect GetCharRect(const char c);
public:
  BBitmap *mBitmap;
  TInt16  mFontType;
  const TUint   mWidth;
  const TUint   mHeight;
};

#endif //GENUS_BFONT_H
