#include "../rcomp.h"

TUint32 RawBitmap::BytesInBitmap() {
  return (TUint32(bytesPerRow) * TUint32(height));
}

TUint32 RawBitmap::OutputSize() {
  TUint32 size = 0;
  size += sizeof(width);
  size += sizeof(height);
  size += sizeof(depth);
  size += sizeof(bytesPerRow);
  size += sizeof(palette_size);
  size += palette_size * 3;
  size += BytesInBitmap();
  return size;
}

void RawBitmap::Dump(const char *pad) {
  printf("%s%-64.64s %dx%d  %d-bit %dc %d raw ", pad,
         filename, width, height, depth, palette_size,
         OutputSize());
#if 0
  for (int i = 0; i < palette_size; i++) {
      printf("%02x: %02x %02x %02x\n", i, palette[i * 3 + 0],
             palette[i * 3 + 1], palette[i * 3 + 2]);
    }
#endif
#if 0
  printf("Pixels:\n");
    for (int y = 0; y < height; y++) {
      printf("%4d    ", y);
      for (int resources = 0; resources < width; resources++) {
        printf("%02x ", pixels[y * width + resources]);
      }
      printf("\n\n");
    }
#endif
}

