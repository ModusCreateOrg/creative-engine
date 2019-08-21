#ifndef BMAPPLAYFIELD_H
#define BMAPPLAYFIELD_H

#include <CreativeEngine.h>

class BMapTileset;

// 32x32 tiles, 64x64 tiles, etc.
const TInt TILESIZE = 32;
class BMapPlayfield : public BPlayfield {
public:
  BMapPlayfield(BViewPort *aViewPort, TUint16 aResourceId);

  virtual ~BMapPlayfield();

public:
  // in pixels
  inline TUint16 TileSize() { return TILESIZE; }
  TUint16 MapWidth() { return mMapWidth * TILESIZE; }
  TUint16 MapHeight() { return mMapHeight * TILESIZE; }
  TUint16 MapMaxX() { return mMapWidth * TILESIZE - SCREEN_WIDTH; }
  TUint16 MapMaxY() { return mMapHeight * TILESIZE - SCREEN_HEIGHT; }

  // in tiles
  TUint16 MapWidthTiles() { return mMapWidth; }
  TUint16 MapHeightTiles() { return mMapHeight; }

public:
  // render section of map to viewport
  void Render();
  // cell is tile number | (attribute << 16)
  TUint32 GetCell(TFloat aWorldX, TFloat aWorldY);

  TUint16 mObjectCount;
  TUint16 *mObjectProgram;

  BBitmap *GetTilesBitmap() { return mTileset; }

protected:
  BViewPort *mViewPort;
  BTileMap *mTileMap;
  BBitmap *mTileset;
  TUint16 mMapWidth, mMapHeight;
  TUint32 *mMapData;
};

#endif //BMAPPLAYFIELD_H
