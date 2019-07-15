#ifndef BMAPPLAYFIELD_H
#define BMAPPLAYFIELD_H

#include <CreativeEngine.h>

class BMapTileset;

// Playfield is a map of width,height 16x16 tiles
// TODO: 32x32 tiles, 64x64 tiles, etc.
class BMapPlayfield : public BPlayfield {
public:
  BMapPlayfield(BViewPort *aViewPort, TUint16 aTilesetSlot, TUint16 aMapSlot, TUint16 aCodesSlot);

  virtual ~BMapPlayfield();

public:
  // in pixels
  inline TUint16 TileSize() { return 16; }
  TUint16 MapWidth() { return mMapWidth * 16; }
  TUint16 MapHeight() { return mMapHeight * 16; }
  TUint16 MapMaxX() { return mMapWidth * 16 - SCREEN_WIDTH; }
  TUint16 MapMaxY() { return mMapHeight * 16 - SCREEN_HEIGHT; }

  // in tiles
  TUint16 MapWidthTiles() { return mMapWidth; }
  TUint16 MapHeightTiles() { return mMapHeight; }

public:
  // render section of map to viewport
  void Render();
  // cell is tile number | (attribute << 16)
  TUint32 GetCell(TFloat aWorldX, TFloat aWorldY);

protected:
  BViewPort *mViewPort;
  TUint16 mTilesetSlot;
  BMapTileset *mTileset;
  TUint16 mMapSlot;
  TUint16 mCodesSlot;
  TUint16 mMapWidth, mMapHeight;
  TUint32 *mMapData;
  TUint16 *mAttributes;
};

#endif //BMAPPLAYFIELD_H
