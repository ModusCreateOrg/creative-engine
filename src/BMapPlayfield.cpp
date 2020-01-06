#include "CreativeEngine.h"
#include "math.h"

struct BO_SLOT {
  TUint16 mObjectCount;
  BObjectProgram *mObjectProgram;
};

static TBool initialized = EFalse;
static BO_SLOT *object_programs[MAX_TILEMAPS];

static void init_cache() {
  if (!initialized) {
    initialized = ETrue;
    for (auto &object_program : object_programs) {
      object_program = ENull;
    }
  }
}

void BMapPlayfield::ResetCache() {
  // Is there a problem with this approach?
  if (initialized) {
    for (auto &object_program : object_programs) {
      delete[] object_program;
      object_program = ENull;
    }
  }
}

void BMapPlayfield::WriteToStream(BMemoryStream *aStream, TInt aNumResources) {
  TInt16 zero = 0;
  for (TInt i = 0; i < aNumResources; i++) {
    if (object_programs[i]) {
      TInt16 oc = object_programs[i]->mObjectCount;
      aStream->Write(&object_programs[i]->mObjectCount, sizeof(mObjectCount));
      BO_SLOT *slot = object_programs[i];
      BObjectProgram *step = slot->mObjectProgram;
      for (TInt o = 0; o < oc; o++) {
        step->WriteToStream(aStream);
        step++;
      }
    } else {
      aStream->Write(&zero, sizeof(zero));
    }
  }
}

void BMapPlayfield::ReadFromStream(BMemoryStream *aStream, TInt aNumResources) {
  init_cache();
  for (TInt i = 0; i < aNumResources; i++) {
    TInt16 oc;
    aStream->Read(&oc, sizeof(oc));
    if (oc) {
      object_programs[i] = new BO_SLOT;
      object_programs[i]->mObjectCount = oc;
      object_programs[i]->mObjectProgram = new BObjectProgram[oc];
      BObjectProgram *p = object_programs[i]->mObjectProgram;
      for (TInt o = 0; o < oc; o++) {
        p[o].ReadFromStream(aStream);
      }
    }
  }
}

/**
  * Tile Mapped Playfield
  *
  * aViewPort: Viewport where playfield is rendered
  * aResourceId: ID of tilemap in Resources.bin
  * aSlot: bitmap slot to use for tileset
  * aCache: true to cache or use cached playfield
  */
BMapPlayfield::BMapPlayfield(BViewPort *aViewPort, TUint16 aResourceId, TInt16 aSlot, TBool aCache) : BPlayfield() {
  init_cache();
  mViewPort = aViewPort;
  mResourceId = aResourceId;
  mSlot = aSlot;

  gResourceManager.ReleaseBitmapSlot(aSlot);
  mTileMap = gResourceManager.LoadTileMap(aResourceId, aSlot);
  //  mTileMap = gResourceManager.LoadTileMap(aResourceId, mSlot);

  BObjectProgram *program = mTileMap->mObjectProgram;
  mObjectCount = mTileMap->mObjectCount;
  if (aCache && object_programs[mResourceId]) {
    mObjectCount = object_programs[mResourceId]->mObjectCount;
    mObjectProgram = object_programs[mResourceId]->mObjectProgram;
  } else {
    object_programs[mResourceId] = new BO_SLOT;
    mObjectProgram = new BObjectProgram[mObjectCount];
    memcpy(mObjectProgram, program, mObjectCount * sizeof(BObjectProgram));
    object_programs[mResourceId]->mObjectCount = mObjectCount;
    object_programs[mResourceId]->mObjectProgram = mObjectProgram;
  }

  mMapWidth = mTileMap->mWidth;
  mMapHeight = mTileMap->mHeight;
  mMapData = &mTileMap->mMapData[0];
  mTileset = mTileMap->mTiles;
  mObjectCount = mTileMap->mObjectCount;
}

BMapPlayfield::~BMapPlayfield() {
  // release Tileset BBitmap
  gResourceManager.ReleaseBitmapSlot(mSlot);
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

static void RenderWidth(TUint8 *dst, TUint8 *src, TInt width) {
  switch (width) {
    case 32:
    RENDER32(dst, src);
      break;
    case 31:
    RENDER31(dst, src);
      break;
    case 30:
    RENDER30(dst, src);
      break;
    case 29:
    RENDER29(dst, src);
      break;
    case 28:
    RENDER28(dst, src);
      break;
    case 27:
    RENDER27(dst, src);
      break;
    case 26:
    RENDER26(dst, src);
      break;
    case 25:
    RENDER25(dst, src);
      break;
    case 24:
    RENDER24(dst, src);
      break;
    case 23:
    RENDER23(dst, src);
      break;
    case 22:
    RENDER22(dst, src);
      break;
    case 21:
    RENDER21(dst, src);
      break;
    case 20:
    RENDER20(dst, src);
      break;
    case 19:
    RENDER19(dst, src);
      break;
    case 18:
    RENDER18(dst, src);
      break;
    case 17:
    RENDER17(dst, src);
      break;
    case 16:
    RENDER16(dst, src);
      break;
    case 15:
    RENDER15(dst, src);
      break;
    case 14:
    RENDER14(dst, src);
      break;
    case 13:
    RENDER13(dst, src);
      break;
    case 12:
    RENDER12(dst, src);
      break;
    case 11:
    RENDER11(dst, src);
      break;
    case 10:
    RENDER10(dst, src);
      break;
    case 9:
    RENDER9(dst, src);
      break;
    case 8:
    RENDER8(dst, src);
      break;
    case 7:
    RENDER7(dst, src);
      break;
    case 6:
    RENDER6(dst, src);
      break;
    case 5:
    RENDER5(dst, src);
      break;
    case 4:
    RENDER4(dst, src);
      break;
    case 3:
    RENDER3(dst, src);
      break;
    case 2:
    RENDER2(dst, src);
      break;
    case 1:
      RENDER1(dst, src);
      break;
    default:
      break;
  }
}
// clang-format on

void BMapPlayfield::Render() {
  TRect &rect = mViewPort->mRect;
  TUint8 *pixels = &gDisplay.renderBitmap->mPixels[0];

  TInt startX = TInt(mViewPort->mWorldX) % TILESIZE,
    startY = TInt(mViewPort->mWorldY) % TILESIZE;

  TInt offRow = TInt(mViewPort->mWorldY) / TILESIZE,
    offCol = TInt(mViewPort->mWorldX) / TILESIZE,
    tilesHigh = TInt(ceil(TFloat(rect.Height()) / TILESIZE)) + (startY ? 1 : 0),
    tilesWide = TInt(ceil(TFloat(rect.Width()) / TILESIZE)) + (startX ? 1 : 0);

  TInt tw = mTileMap->mTiles->Width(); // width of tileset bitmap

  TInt xx = rect.x1;
  for (TInt col = 0; col < tilesWide; col++) {
    TInt yy = rect.y1;
    const TInt offset = yy * SCREEN_WIDTH + xx;
    TUint8 *bm = &pixels[offset];
    for (TInt row = 0; row < tilesHigh; row++) {
      TInt h = MIN(SCREEN_HEIGHT - yy, row ? TILESIZE : TILESIZE - startY),
        w = MIN(SCREEN_WIDTH - xx, col ? TILESIZE : TILESIZE - startX);

      TUint8 *tile = mTileMap->TilePtr(row + offRow, col + offCol);
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
    xx += col ? TILESIZE : TILESIZE - startX;
  }
}

TUint32 BMapPlayfield::GetCell(TFloat aWorldX, TFloat aWorldY) {
  TInt offRow = TInt(aWorldY / TILESIZE),
    offCol = TInt(aWorldX / TILESIZE);

  TUint32 cell = mMapData[offRow * mMapWidth + offCol];
  return cell;
}

TUint32 BMapPlayfield::GetCell(TInt aRow, TInt aCol) {
  TUint32 cell = mMapData[aRow * mMapWidth + aCol];
  return cell;
}

void BMapPlayfield::SetAttribute(TFloat aWorldX, TFloat aWorldY, TUint aAttribute) {
  TInt row = TInt(aWorldY / TILESIZE),
    col = TInt(aWorldX / TILESIZE),
    offset = row * mMapWidth + col;

  TUint32 cell = mMapData[offset];
  cell = LOWORD(cell);
  cell |= aAttribute << 16;
  mMapData[offset] = cell;
}
