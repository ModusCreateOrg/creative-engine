#ifndef MODITE_RAWBITMAP_H
#define MODITE_RAWBITMAP_H

//#include "rcomp.h"

#include <cstdio>
#include <sys/types.h>
#include <cstdarg>
#include <cstdint>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <libgen.h>

#include "BTypes.h"
#include "Memory.h"

class RawBitmap {
public:
  const char *filename;
  uint16_t   width, height, depth, bytesPerRow;
  uint16_t   palette_size; // number of colors
  uint8_t    *palette;
  uint8_t    *pixels;

public:
  uint32_t BytesInBitmap();
  uint32_t OutputSize();
public:
  void Dump(const TBool aShowPalette = false, const TBool aShowPixels = false);
};


#endif //MODITE_RAWBITMAP_H
