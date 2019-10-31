#ifndef MODITE_BSPRITESHEET_H
#define MODITE_BSPRITESHEET_H

#include "BBase.h"
#include "BBitmap.h"

struct BSpriteInfo {
  TInt16 dx, dy;
  TInt16 x1, y1, x2, y2; // rect that perfectly fits the image data
  void Dump(const char *s = ENull) {
    if (s) {
      printf("%s", s);
    }
    printf("%d,%d %d,%d\n", x1, y1, x2, y2);
  }
};

struct BSpriteInfoROM {
  TUint16 resourceId;  // ID of the BBitmap in the binary resource
  TUint16 imageType;   // e.g. IMAGE_32x32
  TUint16 numSprites;  // number of sprites in the sprite sheet
  BSpriteInfo info[1]; // array of numSprites BSpriteInfo structs
};

class BSpriteSheet : public BBase {
public:
  BSpriteSheet(TUint8 *aRom);

public:
  TInt NumSprites() { return mInfo->numSprites; }

  TUint16 BitmapID() { return mInfo->resourceId; }

  BSpriteInfo *GetSpriteInfo(TInt16 aImageNumber) {
    return &mInfo->info[aImageNumber];
  }

public:
  void Dump();

public:
  BSpriteInfoROM *mInfo;
};

#endif //MODITE_BSPRITESHEET_H
