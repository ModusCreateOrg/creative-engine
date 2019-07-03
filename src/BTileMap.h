//
// Created by Michael Schwartz on 2019-07-03.
//

#ifndef BTILEMAP_H
#define BTILEMAP_H

#include <BBase.h>

#define TILE_FLIP (1<<14)
#define TILE_FLOP (1<<15)
#define TILE_INDEX(n) (TUint16((n) & 0x3fff))

class BTileMap : BBase {
public:
  BTileMap(const TUint8 *aData, const TUint16 *aTLC);
  ~BTileMap();
public:
  static BTileMap *LoadFromFiles(const char *path, const char *name);
public:
  TUint16 mWidth, mHeight;
  TUint32 *mMapData;
};


#endif //BTILEMAP_H
