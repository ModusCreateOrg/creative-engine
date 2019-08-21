#include "CreativeEngine.h"

BMapPlayfield::BMapPlayfield(BViewPort *aViewPort, TUint16 aResourceId) : BPlayfield() {
  mViewPort = aViewPort;
  mTileMap = gResourceManager.LoadTileMap(aResourceId);
  mMapWidth = mTileMap->mWidth;
  mMapHeight = mTileMap->mHeight;
  mMapData = &mTileMap->mMapData[0];
  mTileset = mTileMap->mTiles;
  mObjectProgram = mTileMap->mObjectProgram;
  mObjectCount= mTileMap->mObjectCount;
  printf("BMapPlayfield: %d Objects\n", mObjectCount);
}

BMapPlayfield::~BMapPlayfield() {
  delete mTileMap;
}

void BMapPlayfield::Render() {
  TRect& rect = mViewPort->mRect;
  TInt offRow = TInt(mViewPort->mWorldY / TileSize()),
    offCol = TInt(mViewPort->mWorldX / TileSize()),
    tilesHigh = rect.Height() / TileSize(),
    tilesWide = rect.Width() / TileSize();


  for (TInt row=0; row<tilesHigh; row++) {
    for (TInt col=0; col<tilesWide; col++) {
      const TUint8 *tile = mTileMap->TilePtr(row + offRow, col + offCol);
      const TInt offset = (rect.y1 + row*TILESIZE) * SCREEN_WIDTH + rect.x1 + col*TILESIZE;
      TUint8 *bm = &gDisplay.renderBitmap->mPixels[offset];
      for (TInt y=0; y<TILESIZE; y++) {
        for (TInt x=0; x<TILESIZE; x++) {
          bm[x] = tile[x];
        }
        bm += SCREEN_WIDTH;
        tile += mTileset->Width();
      }
    }
  }
}

TUint32 BMapPlayfield::GetCell(TFloat aWorldX, TFloat aWorldY) {
  TInt offRow = TInt(aWorldY / TileSize()),
    offCol = TInt(aWorldX / TileSize());

  TUint32 cell = mMapData[offRow * mMapWidth + offCol];
  return cell;
}
