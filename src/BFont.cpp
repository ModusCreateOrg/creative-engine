#include "BFont.h"
#include "BBitmap.h"

BFont::BFont(TInt16 aBitmapSlot) {
  mBitmapSlot = aBitmapSlot;
}

BFont::~BFont() {

}

TRect BFont::GetCharRect(const char c) {
  TInt row = c / 16;
  TInt col = c % 16;

  return TRect(col*8, row*8, col*8+7, row*8+7);
}
