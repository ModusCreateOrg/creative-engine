//
// Created by Michael Schwartz on 9/27/18.
//

#ifndef GENUS_BFONT_H
#define GENUS_BFONT_H

#include "BTypes.h"

class BBitmap;

/**
 * BFont references the slots in a bitmap that contain fonts.
 */
class BFont {
public:
  BFont(BBitmap *b);
  ~BFont();
public:
  TRect GetCharRect(const char c);
public:
  BBitmap *mBitmap;
};


#endif //GENUS_BFONT_H
