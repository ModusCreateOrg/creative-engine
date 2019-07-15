#include "CreativeEngine.h"

class BMapTileset : public BBitmap {
public:
  TUint8 *TilePtr(TUint32 aTileNumber) {
    const TInt tw = Width() / 16,
      row = aTileNumber / tw,
      col = aTileNumber % tw;

    const TInt offset = row * 16 * Width() + col*16;
    return &mPixels[offset];
  };
};

struct MAP {
  TUint8 token[4];
  TUint16 width;
  TUint16 height;
  TUint32 map[];
};

BMapPlayfield::BMapPlayfield(BViewPort *aViewPort, TUint16 aTilesetSlot, TUint16 aMapSlot, TUint16 aCodesSlot) : BPlayfield() {
  mViewPort = aViewPort;
  mTilesetSlot = aTilesetSlot;
  mMapSlot = aMapSlot;
  mCodesSlot = aCodesSlot;
  mTileset = (BMapTileset *)gResourceManager.GetBitmap(mTilesetSlot);
  gDisplay.SetPalette(mTileset);

  BRaw *rawMap = gResourceManager.GetRaw(aMapSlot);
  auto *pw = (MAP *) &rawMap->mData[0];
  mMapWidth = pw->width;
  mMapHeight = pw->height;
  mMapData = &pw->map[0];

  BRaw *rawAttributes = gResourceManager.GetRaw(aCodesSlot);
  mAttributes = (TUint16 *) &rawAttributes->mData[0];
}

BMapPlayfield::~BMapPlayfield() {
  //
}

void BMapPlayfield::Render() {
  TRect& rect = mViewPort->mRect;
  TInt offRow = TInt(mViewPort->mWorldY / TileSize()),
    offCol = TInt(mViewPort->mWorldX / TileSize()),
    tilesHigh = rect.Height() / TileSize(),
    tilesWide = rect.Width() / TileSize();


  for (TInt row=0; row<tilesHigh; row++) {
    const TInt offset = (row +offRow) * mMapWidth + offCol;
    TUint32 *map = &mMapData[offset];
    for (TInt col=0; col<tilesWide; col++) {
      const TUint8 *tile = mTileset->TilePtr(TUint32(map[col]) & TUint32(0xffff));
      const TInt offset = (rect.y1 + row*16) * SCREEN_WIDTH + rect.x1 + col*16;
      TUint8 *bm = &gDisplay.renderBitmap->mPixels[offset];
      for (TInt y=0; y<16; y++) {
        for (TInt x=0; x<16; x++) {
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
  TUint32 attr = TUint32(mAttributes[cell&0xffff]);
  return (cell & TUint32(0xffff)) | (attr << 16);
}