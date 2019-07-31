#include "CreativeEngine.h"

BTileMap::BTileMap(void *aRomData) {
  TUint16 *rom = (TUint16 *)aRomData;
  mTiles = gResourceManager.LoadBitmap(rom[0]);
  mWidth = rom[1]; // romTileMap->mWidth;
  mHeight = rom[2]; // romTileMap->mHeight;
  mMapData = new TUint32[mWidth * mHeight];
  memcpy(mMapData, &rom[3], mWidth * mHeight * sizeof(TUint32));
}

BTileMap::~BTileMap() {
  delete mTiles;
  delete [] this->mMapData;
}


TUint8 *BTileMap::TilePtr(TInt aRow, TInt aCol) {
  const TInt index = aRow * mWidth + aCol,
  tileNumber = mMapData[index] & 0xffff;

  const TInt tw = mTiles->Width() / TILESIZE,
    row = tileNumber / tw,
    col = tileNumber % tw;

  const TInt offset = row * TILESIZE * mTiles->Width() + col*TILESIZE;
  return &mTiles->mPixels[offset];
}

