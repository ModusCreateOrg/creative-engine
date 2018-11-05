#ifndef GENUS_TRGB_H
#define GENUS_TRGB_H

#include "BTypes.h"

struct TRGB {
  TUint8 r, g, b;

public:
  TRGB() {
    r = g = b = 0;
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

  TRGB(TRGB &aOther) {
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

  void Set(TRGB &aOther) {
    r = aOther.r;
    g = aOther.g;
    b = aOther.b;
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
  TUint32 rgb888() { return (r << 16) | (g << 8) | (b << 0); }

  TUint16 rgb565() {
    return ((TUint16(r & 0b11111000) << 8)) | ((TUint16(g & 0b11111100) << 3)) | (TUint16(b) >> 3);
  }

  TUint16 bgr565() {
    return ((b & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (r >> 3);
  }

  TUint16 brg565() {
    return ((b & 0b11111000) << 8) | ((r & 0b11111100) << 3) | (g >> 3);
  }

public:
  void Dump() {
    printf("%02x:%02x:%02x\n", r, g, b);
  }
};

#endif //GENUS_TRGB_H
