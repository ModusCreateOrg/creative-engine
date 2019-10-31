#ifndef RCOMP_SPRITESHEET_H
#define RCOMP_SPRITESHEET_H

#include "rcomp.h"

#if 0
struct BSpriteInfo {
  TInt dx, dy;  // delta from upper left of sprite to upper left of fit rect
  TInt x1, y1, x2, y2; // rect that perfectly fits the image data
};

struct BSpriteInfoROM {
  TUint16 resourceId;
  TUint16 imageType;
  TUint16 numSprites;
  BSpriteInfo info[1];
};
#endif

class BSpriteInfo;

class SpriteSheet {
public:
  SpriteSheet(char *aDimensions, char *aBitmap);

  ~SpriteSheet();

public:
  void Write(ResourceFile &r);

protected:
  BMPFile *bmp;
  BSpriteInfo *info;
  TUint16 resourceId;
  TInt16 image_type;
  TUint16 num_sprites;
};

#endif //RCOMP_SPRITESHEET_H
