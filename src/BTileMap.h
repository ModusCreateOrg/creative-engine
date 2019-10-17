#ifndef BTILEMAP_H
#define BTILEMAP_H

#include <BBase.h>
#include <BBitmap.h>

#define TILE_INDEX(n) (TUint16((n) & 0xffff))

struct BObjectProgram {
  TUint32 mCode;
  TUint16 mRow, mCol;
};

class BTileMap : BBase {
public:
  BTileMap(void *aRomData, TInt16 aTilesetSlot);

  ~BTileMap();

public:
  TUint32 *TilePtr(TInt aRow, TInt aCol);

public:
  TUint16        mWidth, mHeight;
  TUint32        *mMapData;
  TUint16        mObjectCount;
  BObjectProgram *mObjectProgram;
  TUint16        mTilesetSlot;
  BBitmap        *mTiles;
};

#endif //BTILEMAP_H
