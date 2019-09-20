#include "CreativeEngine.h"
#include "math.h"

BMapPlayfield::BMapPlayfield(BViewPort *aViewPort, TUint16 aResourceId, TInt16 aSlot) : BPlayfield() {
  mViewPort      = aViewPort;
  mTileMap       = gResourceManager.LoadTileMap(aResourceId, aSlot);
  mMapWidth      = mTileMap->mWidth;
  mMapHeight     = mTileMap->mHeight;
  mMapData       = &mTileMap->mMapData[0];
  mTileset       = mTileMap->mTiles;
  mObjectProgram = mTileMap->mObjectProgram;
  mObjectCount   = mTileMap->mObjectCount;
  printf("BMapPlayfield: %d Objects\n", mObjectCount);
}

BMapPlayfield::~BMapPlayfield() {
  delete mTileMap;
}

// clang-format off
#define RENDER1(dst, src) *dst++ = *src++;
#define RENDER2(dst, src) RENDER1(dst, src) RENDER1(dst, src)
#define RENDER3(dst, src) RENDER2(dst, src) RENDER1(dst, src)
#define RENDER4(dst, src) RENDER3(dst, src) RENDER1(dst, src)
#define RENDER5(dst, src) RENDER4(dst, src) RENDER1(dst, src)
#define RENDER6(dst, src) RENDER5(dst, src) RENDER1(dst, src)
#define RENDER7(dst, src) RENDER6(dst, src) RENDER1(dst, src)
#define RENDER8(dst, src) RENDER7(dst, src) RENDER1(dst, src)
#define RENDER9(dst, src) RENDER8(dst, src) RENDER1(dst, src)
#define RENDER10(dst, src) RENDER9(dst, src) RENDER1(dst, src)
#define RENDER11(dst, src) RENDER10(dst, src) RENDER1(dst, src)
#define RENDER12(dst, src) RENDER11(dst, src) RENDER1(dst, src)
#define RENDER13(dst, src) RENDER12(dst, src) RENDER1(dst, src)
#define RENDER14(dst, src) RENDER13(dst, src) RENDER1(dst, src)
#define RENDER15(dst, src) RENDER14(dst, src) RENDER1(dst, src)
#define RENDER16(dst, src) RENDER15(dst, src) RENDER1(dst, src)
#define RENDER17(dst, src) RENDER16(dst, src) RENDER1(dst, src)
#define RENDER18(dst, src) RENDER17(dst, src) RENDER1(dst, src)
#define RENDER19(dst, src) RENDER18(dst, src) RENDER1(dst, src)
#define RENDER20(dst, src) RENDER19(dst, src) RENDER1(dst, src)
#define RENDER21(dst, src) RENDER20(dst, src) RENDER1(dst, src)
#define RENDER22(dst, src) RENDER21(dst, src) RENDER1(dst, src)
#define RENDER23(dst, src) RENDER22(dst, src) RENDER1(dst, src)
#define RENDER24(dst, src) RENDER23(dst, src) RENDER1(dst, src)
#define RENDER25(dst, src) RENDER24(dst, src) RENDER1(dst, src)
#define RENDER26(dst, src) RENDER25(dst, src) RENDER1(dst, src)
#define RENDER27(dst, src) RENDER26(dst, src) RENDER1(dst, src)
#define RENDER28(dst, src) RENDER27(dst, src) RENDER1(dst, src)
#define RENDER29(dst, src) RENDER28(dst, src) RENDER1(dst, src)
#define RENDER30(dst, src) RENDER29(dst, src) RENDER1(dst, src)
#define RENDER31(dst, src) RENDER30(dst, src) RENDER1(dst, src)
#define RENDER32(dst, src) RENDER31(dst, src) RENDER1(dst, src)

static void RenderWidth(TUint32 *dst, TUint32 *src, TInt width) {
  switch (width) {
    case 32: RENDER32(dst, src); break;
    case 31: RENDER31(dst, src); break;
    case 30: RENDER30(dst, src); break;
    case 29: RENDER29(dst, src); break;
    case 28: RENDER28(dst, src); break;
    case 27: RENDER27(dst, src); break;
    case 26: RENDER26(dst, src); break;
    case 25: RENDER25(dst, src); break;
    case 24: RENDER24(dst, src); break;
    case 23: RENDER23(dst, src); break;
    case 22: RENDER22(dst, src); break;
    case 21: RENDER21(dst, src); break;
    case 20: RENDER20(dst, src); break;
    case 19: RENDER19(dst, src); break;
    case 18: RENDER18(dst, src); break;
    case 17: RENDER17(dst, src); break;
    case 16: RENDER16(dst, src); break;
    case 15: RENDER15(dst, src); break;
    case 14: RENDER14(dst, src); break;
    case 13: RENDER13(dst, src); break;
    case 12: RENDER12(dst, src); break;
    case 11: RENDER11(dst, src); break;
    case 10: RENDER10(dst, src); break;
    case 9: RENDER9(dst, src); break;
    case 8: RENDER8(dst, src); break;
    case 7: RENDER7(dst, src); break;
    case 6: RENDER6(dst, src); break;
    case 5: RENDER5(dst, src); break;
    case 4: RENDER4(dst, src); break;
    case 3: RENDER3(dst, src); break;
    case 2: RENDER2(dst, src); break;
    case 1: RENDER1(dst, src); break;
    default:
      break;
  }
}
// clang-format on

void BMapPlayfield::Render() {
  TRect &rect = mViewPort->mRect;
  TUint32 *pixels = &gDisplay.renderBitmap->mPixels[0];

  TInt startX = TInt(mViewPort->mWorldX) % TILESIZE,
       startY = TInt(mViewPort->mWorldY) % TILESIZE;

  TInt offRow    = TInt(mViewPort->mWorldY) / TILESIZE,
       offCol    = TInt(mViewPort->mWorldX) / TILESIZE,
       tilesHigh = TInt(ceil(TFloat(rect.Height()) / TILESIZE)) + (startY ? 1 : 0),
       tilesWide = TInt(ceil(TFloat(rect.Width()) / TILESIZE)) + (startX ? 1 : 0);

  TInt tw = mTileMap->mTiles->Width();  // width of tileset bitmap

  TInt      xx  = rect.x1;
  for (TInt col = 0; col < tilesWide; col++) {
    TInt       yy     = rect.y1;
    const TInt offset = yy * SCREEN_WIDTH + xx;
    TUint32     *bm    = &pixels[offset];
    for (TInt  row    = 0; row < tilesHigh; row++) {
      TInt h = MIN(SCREEN_HEIGHT - yy, row ? TILESIZE : TILESIZE - startY),
           w = MIN(SCREEN_WIDTH - xx, col ? TILESIZE : TILESIZE - startX);

      TUint32 *tile = mTileMap->TilePtr(row + offRow, col + offCol);
      if (row == 0) {
        tile = &tile[startY * tw];
      }
      if (col == 0) {
        tile = &tile[startX];
      }

      for (TInt y = 0; y < h; y++) {
        RenderWidth(&bm[0], &tile[0], w);
        bm += SCREEN_WIDTH;
        tile += tw;
        yy++;
      }
    }
    xx += col ? TILESIZE : TILESIZE-startX;
  }
}

TUint32 BMapPlayfield::GetCell(TFloat aWorldX, TFloat aWorldY) {
  TInt offRow = TInt(aWorldY / TILESIZE),
       offCol = TInt(aWorldX / TILESIZE);

  TUint32 cell = mMapData[offRow * mMapWidth + offCol];
  return cell;
}
