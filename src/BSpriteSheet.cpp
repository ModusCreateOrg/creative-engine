#include "BSpriteSheet.h"

BSpriteSheet::BSpriteSheet(TUint8 *aROM) {
  mInfo = (BSpriteInfoROM *)&aROM[0];
//  Dump();
}

void BSpriteSheet::Dump() {
  printf("\nBSpriteSheet @%p - %d Sprites\n", this, mInfo->numSprites);
  for (TInt i=0; i<mInfo->numSprites; i++) {
    char buf[256];
    sprintf(buf, "info [%-5d]   = ", i);
    mInfo->info[i].Dump(buf);
  }
}