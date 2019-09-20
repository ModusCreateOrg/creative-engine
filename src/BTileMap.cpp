#include "CreativeEngine.h"

BTileMap::BTileMap(void *aRomData, TInt16 aTilesetSlot) {
  TUint16 *rom = (TUint16 *) aRomData;
  WordDump(rom, 32);
  mTilesetSlot = aTilesetSlot;
  if (!gResourceManager.LoadBitmap(*rom++, aTilesetSlot, IMAGE_32x32)) {
    Panic("Can't loadBitmap(%d)\n", mTilesetSlot);
  }
  mTiles         = gResourceManager.GetBitmap(aTilesetSlot);
  mObjectCount   = *rom++;
  mObjectProgram = (BObjectProgram *) &rom[0];
  rom            = (TUint16 *) &mObjectProgram[mObjectCount];
  mWidth         = *rom++;
  mHeight        = *rom++;
  mMapData       = new TUint32[mWidth * mHeight];
  memcpy(mMapData, &rom[0], mWidth * mHeight * sizeof(TUint32));
  printf("TILEMAP is %d by %d\n", mWidth, mHeight);
}

BTileMap::~BTileMap() {
  delete mTiles;
  delete[] this->mMapData;
}

TUint32 *BTileMap::TilePtr(TInt aRow, TInt aCol) {
  const TInt index      = aRow * mWidth + aCol,
             tileNumber = mMapData[index] & TUint32(0xffff);

  const TInt tw  = mTiles->Width() / TILESIZE,
             row = tileNumber / tw,
             col = tileNumber % tw;

  const TInt offset = row * TILESIZE * mTiles->Width() + col * TILESIZE;
  return &mTiles->mPixels[offset];
}

