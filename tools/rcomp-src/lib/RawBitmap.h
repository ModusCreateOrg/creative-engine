#ifndef MODITE_RAWBITMAP_H
#define MODITE_RAWBITMAP_H

#include "rcomp.h"

#include "../../../src/BTypes.h"
#include "../../../src/Memory.h"

class RawBitmap {
public:
  char *filename;
  TUint16 width, height, depth, bytesPerRow;
  TUint16 palette_size; // number of colors
  TUint8 *palette;
  TUint8 *pixels;

public:
  TUint32 BytesInBitmap();
  TUint32 OutputSize();

public:
  void Dump(const char *aPad = "");
};

#endif //MODITE_RAWBITMAP_H
