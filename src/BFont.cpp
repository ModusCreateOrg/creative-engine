#include "BFont.h"
#include "BBitmap.h"

BFont::BFont(BBitmap *b, TInt16 aFontType) {
  mBitmap = b;
  mFontType = aFontType;

  static const TUint8 widthTable[] = {
    8, 16, 32, 64, 8, 16, 8, 32, 8, 64, 16, 32, 16, 64, 32, 64, 32
  };
  static const TUint8 heightTable[] = {
    8, 16, 32, 64, 16, 8, 32, 8, 64, 8, 32, 16, 64, 16, 64, 32, 40
  };

  mWidth = TInt(widthTable[aFontType]);
  mHeight = TInt(heightTable[aFontType]);
}

BFont::~BFont() {

}

TRect BFont::GetCharRect(const char c) {
  TInt row = c / 16;
  TInt col = c % 16;
  TInt x1 = col * mWidth;
  TInt y1 = row * mHeight;

  return TRect(x1, y1, x1 + (mWidth - 1), y1 + (mHeight - 1));
}
