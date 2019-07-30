#include "CreativeEngine.h"

struct ROM_TILEMAP {
  TUint16 mBitmapResourceId;
  TUint16 mWidth;
  TUint16 mHeight;
  TUint32 mMapData[];
};

BTileMap::BTileMap(void *aRomData) {
  TUint16 *rom = (TUint16 *)aRomData;
//  ROM_TILEMAP *romTileMap = (ROM_TILEMAP *)aRomData;
  WordDump((TUint16 *)rom, 16);
  ByteDump((TUint8 *)rom, 16);
  mTiles = gResourceManager.LoadBitmap(rom[0]);
  mWidth = rom[1]; // romTileMap->mWidth;
  mHeight = rom[2]; // romTileMap->mHeight;
  mMapData = new TUint32[mWidth * mHeight];
//  WordDump((TUint16 *)&romTileMap->mMapData[0], 16);
  memcpy(mMapData, &rom[3], mWidth * mHeight * sizeof(TUint32));
  LongDump(&mMapData[0], 16);
  printf("\n\n");
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

