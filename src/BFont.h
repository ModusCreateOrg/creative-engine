//
// Created by Michael Schwartz on 9/27/18.
//

#ifndef GENUS_BFONT_H
#define GENUS_BFONT_H

#include "BTypes.h"


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
