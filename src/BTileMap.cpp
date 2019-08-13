#include "CreativeEngine.h"

BTileMap::BTileMap(void *aRomData) {
  TUint16 *rom = (TUint16 *)aRomData;
  WordDump(rom, 50);
  mTiles = gResourceManager.LoadBitmap(rom[0]);
  mObjectCount = rom[1];
  mObjectProgram= &rom[2];
  TInt programEnd = 3 + mObjectCount+3+2;
  mWidth = rom[programEnd]; // romTileMap->mWidth;
  mHeight = rom[programEnd+1]; // romTileMap->mHeight;
  mMapData = new TUint32[mWidth * mHeight];
  memcpy(mMapData, &rom[programEnd+2], mWidth * mHeight * sizeof(TUint32));
  printf("TILEMAP is %d by %d\n", mWidth, mHeight);
  LongDump(mMapData, 32);
}

BTileMap::~BTileMap() {
  delete mTiles;
  delete [] this->mMapData;
}


TUint8 *BTileMap::TilePtr(TInt aRow, TInt aCol) {
  const TInt index = aRow * mWidth + aCol,
  tileNumber = mMapData[index] & TUint32(0xffff);

  const TInt tw = mTiles->Width() / TILESIZE,
    row = tileNumber / tw,
    col = tileNumber % tw;

  const TInt offset = row * TILESIZE * mTiles->Width() + col*TILESIZE;
  return &mTiles->mPixels[offset];
}

