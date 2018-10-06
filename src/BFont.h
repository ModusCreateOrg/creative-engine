//
// Created by Michael Schwartz on 9/27/18.
//

#ifndef GENUS_BFONT_H
#define GENUS_BFONT_H

#include "BTypes.h"

/**
 * BFont references the slots in a bitmap that contain fonts.
 */
class BFont {
public:
  BFont(TInt16 aBitmapSlot);
  ~BFont();
public:
  TRect GetCharRect(const char c);
public:
  TInt16 mBitmapSlot;
};


#endif //GENUS_BFONT_H
