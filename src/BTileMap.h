#ifndef BTILEMAP_H
#define BTILEMAP_H

#include <BBase.h>
#include <BBitmap.h>
#include <BMemoryStream.h>

#define TILE_INDEX(n) (TUint16((n)&0xffff))

struct BObjectProgram {
  TUint32 mCode;
  TUint16 mRow, mCol;
  void WriteToStream(BMemoryStream *aStream) {
    aStream->Write(&mCode, sizeof(mCode));
    aStream->Write(&mRow, sizeof(mRow));
    aStream->Write(&mCol, sizeof(mCol));
  }
  void ReadFromStream(BMemoryStream *aStream) {
    aStream->Read(&mCode, sizeof(mCode));
    aStream->Read(&mRow, sizeof(mRow));
    aStream->Read(&mCol, sizeof(mCol));
  }
  void Dump(TInt ndx) {
    printf("%5d %24.24s: %08x(%d:%d) %d,%d\n", ndx, "Code, row,col", mCode, LOWORD(mCode), HIWORD(mCode), mRow, mCol);
  }
};

class BTileMap : BBase {
public:
  BTileMap(void *aRomData, TInt16 aTilesetSlot);

  ~BTileMap();

public:
  TUint8 *TilePtr(TInt aRow, TInt aCol);

public:
  TUint16 mWidth, mHeight;
  TUint32 *mMapData;
  TUint16 mObjectCount;
  BObjectProgram *mObjectProgram;
  TUint16 mTilesetId, mTilesetSlot;
  BBitmap *mTiles;
};

#endif //BTILEMAP_H
