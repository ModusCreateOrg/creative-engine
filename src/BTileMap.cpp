#include "CreativeEngine.h"

BTileMap::BTileMap(void *aRomData, TInt16 aTilesetSlot) {
  TUint16 *rom = (TUint16 *)aRomData;
  mTilesetSlot = aTilesetSlot;
  mTilesetId= *rom++;
  if (!gResourceManager.LoadBitmap(mTilesetId, mTilesetSlot, IMAGE_32x32)) {
    Panic("Can't loadBitmap(%d)\n", mTilesetSlot);
  }
  mTiles = gResourceManager.GetBitmap(aTilesetSlot);
  mObjectCount = *rom++;
  mObjectProgram = (BObjectProgram *)&rom[0];
  rom = (TUint16 *)&mObjectProgram[mObjectCount];
  mWidth = *rom++;
  mHeight = *rom++;
  mMapData = new TUint32[mWidth * mHeight];
  memcpy(mMapData, &rom[0], mWidth * mHeight * sizeof(TUint32));
  printf("TILEMAP is %d by %d\n", mWidth, mHeight);

  // Handle different bitmap color depths
  if (gDisplay.renderBitmap->Depth() != mTiles->Depth()) {
    // Convert 8bit color tilemap to a 32bit canvas
    if (gDisplay.renderBitmap->Depth() == 32) {
      TUint32 *pixels = &mTiles->mPixels[0];
      for (TInt y = 0; y < mTiles->Height(); y++) {
        for (TInt x = 0; x < mTiles->Width(); x++, pixels++) {
          *pixels = mTiles->ReadColor(x, y).rgb888();
        }
      }
      memcpy(mTiles->mPixels, pixels, mWidth * mHeight * sizeof(TUint32));
    }
  }
}

BTileMap::~BTileMap() {
  delete mTiles;
  delete[] this->mMapData;
}

TUint32 *BTileMap::TilePtr(TInt aRow, TInt aCol) {
  const TInt index = aRow * mWidth + aCol,
             tileNumber = mMapData[index] & TUint32(0xffff);

  const TInt tw = mTiles->Width() / TILESIZE,
             row = tileNumber / tw,
             col = tileNumber % tw;

  const TInt offset = row * TILESIZE * mTiles->Width() + col * TILESIZE;

  return &mTiles->mPixels[offset];
}
