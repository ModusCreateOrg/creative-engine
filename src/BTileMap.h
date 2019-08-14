#ifndef BTILEMAP_H
#define BTILEMAP_H

#include <BBase.h>

#define TILE_INDEX(n) (TUint16((n) & 0xffff))

class BTileMap : BBase {
public:
  BTileMap(void *aRomData);
  ~BTileMap();
public:
  TUint8 *TilePtr(TInt aRow, TInt aCol);

public:
  TUint16 mWidth, mHeight;
  TUint32 *mMapData;
  TUint16 mObjectCount;
  TUint16 *mObjectProgram;
  BBitmap *mTiles;
};

#endif //BTILEMAP_H
