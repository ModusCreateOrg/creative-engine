#ifndef GENUS_TRGB_H
#define GENUS_TRGB_H

#include "BBase.h"
#include "BTypes.h"

struct TRGB {
  TInt16 r, g, b;

public:
  TRGB() {
    r = g = b = 0;
  }

  TRGB(TInt aVal) {
    r = g = b = aVal;
  }

  TRGB(TUint32 aColor) {
    r = TUint8((aColor >> 16) & 0xff);
    g = TUint8((aColor >> 8) & 0xff);
    b = TUint8((aColor >> 0) & 0xff);
  }

  TRGB(TUint8 aRed, TUint8 aGreen, TUint8 aBlue) {
    r = aRed;
    g = aGreen;
    b = aBlue;
  }

  TRGB(const TRGB &aOther) {
    r = aOther.r;
    g = aOther.g;
    b = aOther.b;
  }

public:
  void Set(TUint8 aRed, TUint8 aGreen, TUint8 aBlue) {
    r = aRed;
    g = aGreen;
    b = aBlue;
  }

  void Set(const TRGB &aOther) {
    r = aOther.r;
    g = aOther.g;
    b = aOther.b;
  }

  void Set(TUint32 aColor) {
    r = TUint8((aColor >> 16) & 0xff);
    g = TUint8((aColor >> 8) & 0xff);
    b = TUint8((aColor >> 0) & 0xff);
  }

  void Scale(TUint aScale) {
    r = TUint8(r * aScale) >> 8;
    g = TUint8(g * aScale) >> 8;
    b = TUint8(b * aScale) >> 8;
  }

  void SetScaled(TUint8 aRed, TUint8 aGreen, TUint8 aBlue, TUint aScale) {
    r = TUint8(aRed * aScale) >> 8;
    g = TUint8(aGreen * aScale) >> 8;
    b = TUint8(aBlue * aScale) >> 8;
  }

public:
  TUint32 rgb888() const { return (TUint8(r) << 16) | (TUint8(g) << 8) | (TUint8(b) << 0); }

  TUint16 rgb565() const {
    return ((TUint16(r & 0b11111000) << 8)) | ((TUint16(g & 0b11111100) << 3)) | (TUint16(b) >> 3);
  }

  TUint16 bgr565() const {
    return ((b & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (r >> 3);
  }

  TUint16 brg565() const {
    return ((b & 0b11111000) << 8) | ((r & 0b11111100) << 3) | (g >> 3);
  }

  TBool operator==(const TRGB &aOther) {
    return (r == aOther.r && g == aOther.g && b == aOther.b);
  }

  TBool operator==(TInt aInt) const {
    return (r == aInt && g == aInt && b == aInt);
  }

public:
  void Dump() {
#ifndef PRODUCTION
#if (defined(__XTENSA__) && defined(DEBUGME)) || !defined(__XTENSA__)
    printf("%02x:%02x:%02x\n", r, g, b);
#endif
#endif
  }
};

typedef struct TRGB TPalette;

#endif //GENUS_TRGB_H
