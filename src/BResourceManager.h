#ifndef BRESOURCEMANAGER_H
#define BRESOURCEMANAGER_H

/**
 * Resource Management.
 *
 * Resources are arbitrary files that are packed into a bundle of resource by
 * the resource compiler tool (tools/rcomp).
 *
 * The bundle is then included in the app/game as raw binary, using the
 * COMPONENT_EMBED_FILES define in the component.mk for the game.
 */
#include "BBase.h"

class BBitmap;

// Each resource that is loaded requires allocated RAM, so we don't want to just load them all
// from the FLASH/ROM/RODATA all at once.

// Maximum number of resources that can be loaded at one time
static const int MAX_RESOURCE = 1024;

// Image type, passed as argument to LoadBitmap().

static const TInt16 IMAGE_ENTIRE = 0; // image is the entire bitmap in width, height
static const TInt16 IMAGE_8x8 = 1; // bitmap contains individual images, each 8x8, on a grid
static const TInt16 IMAGE_16x16 = 2; // "" 16x16 on a grid
static const TInt16 IMAGE_32x32 = 3; // etc.
static const TInt16 IMAGE_64x64 = 4;
static const TInt16 IMAGE_128x128 = 5;
static const TInt16 IMAGE_256x256 = 6;

static const TInt16 IMAGE_8x16 = 7;
static const TInt16 IMAGE_16x8 = 8;
static const TInt16 IMAGE_8x32 = 9;
static const TInt16 IMAGE_32x8 = 10;
static const TInt16 IMAGE_8x64 = 11;
static const TInt16 IMAGE_64x8 = 12;
static const TInt16 IMAGE_8x128 = 13;
static const TInt16 IMAGE_128x8 = 14;
static const TInt16 IMAGE_8x256 = 15;
static const TInt16 IMAGE_256x8 = 16;

static const TInt16 IMAGE_16x32 = 17;
static const TInt16 IMAGE_32x16 = 18;
static const TInt16 IMAGE_16x64 = 19;
static const TInt16 IMAGE_64x16 = 20;
static const TInt16 IMAGE_16x128 = 21;
static const TInt16 IMAGE_128x16 = 22;
static const TInt16 IMAGE_16x256 = 23;
static const TInt16 IMAGE_256x16 = 24;

static const TInt16 IMAGE_32x64 = 25;
static const TInt16 IMAGE_64x32 = 26;
static const TInt16 IMAGE_32x128 = 27;
static const TInt16 IMAGE_128x32 = 28;
static const TInt16 IMAGE_32x256 = 29;
static const TInt16 IMAGE_256x32 = 30;

static const TInt16 IMAGE_64x128 = 31;
static const TInt16 IMAGE_128x64 = 32;
static const TInt16 IMAGE_64x256 = 33;
static const TInt16 IMAGE_256x64 = 34;

static const TInt16 IMAGE_128x256 = 35;
static const TInt16 IMAGE_256x128 = 36;
struct BitmapSlot;
struct RawSlot;

// maximum number of bitmaps loaded at any given time
#define MAX_BITMAP_SLOTS 64
#define MAX_RAW_SLOTS 64
#define MAX_PRELOADED_BITMAPS 64

class BRaw : public BBase {
public:
  BRaw(TUint8 *aData) {
    TUint32 *ptr = (TUint32 *) aData;
    mSize = *ptr++;
    mData = (TUint8 *) &ptr[0];
  }

public:
  TUint32 mSize;
  TUint8 *mData;
};

//
class BResourceManager : public BBase {
public:
  // construct a resource manager from data in ROM (FLASH)
  BResourceManager(TAny *aROM);

  ~BResourceManager();

public:
  // Load a bitmap from Flash into a slot.  From this point forward, refer to the bitmap
  // by the slot ID in application code.
  TBool LoadBitmap(TInt16 aReosurceId, TInt16 aSlotId, TInt16 aImageType = IMAGE_ENTIRE);

  // Preload a bitmap from Flash into preload cache
  // typically you will preload as many as makes sense to speed up game level loading.
  TBool PreloadBitmap(TInt16 aResourceId);

  // Assign an already loaded BBitmap to a slot.  In theory, these would be RAM based bitamaps.
  // These bitmaps WILL be released, if not cached, meaning they will be (operator) delete.
  TBool SetBitmap(BBitmap *aBitmap, TInt16 aSlotId, TInt16 aImageType = IMAGE_ENTIRE);

  // Unload a bitmap from a slot, releasing any resources/memory it uses.
  // Caveat Progammer: This will NOT unload a cached bitmap!
  TBool ReleaseBitmapSlot(TInt16 aSlotId);

  // Unload all non-cached bitmaps slots.
  void ReleaseBitmapSlots();

  // Cache an already loaded bitmap, so ReleaseBitmaps() won't free it.
  TBool CacheBitmapSlot(TInt16 aSlotId, TBool aCacheIt = ETrue);

  // Clear (or set) cached status of all loaded bitmaps
  void ClearBitmapCache(TBool aCacheIt = EFalse);

  // Get an already loaded bitmap, given the slot number
  BBitmap *GetBitmap(TInt16 aSlotId);

  // Get width of image at slot.  If bitmap is a grid of images, then the width of the grid is returned.
  // That is, if the bitmap has 32x32 images on it, then 32 is returned.
  TInt BitmapWidth(TInt aSlotId);

  // Get height of image at slot.  If bitmap is a grid of images, then the height of the grid is returned.
  // That is, if the bitmap has 32x32 images on it, then 32 is returned.
  TInt BitmapHeight(TInt aSlotId);

public:
  TBool LoadRaw(TInt16 aResourceId, TInt16 aSlotId);

  TBool ReleaseRawSlot(TInt16 aSlotId);

  void ReleaseRawSlots();

  TBool CacheRawSlot(TInt16 aSlotId, TBool aCacheIt = ETrue);

  void ClearRawCache(TBool aCacheIt = EFalse);

  BRaw *GetRaw(TInt16 aSlotId);

public:
  void Dump();

protected:
  TAny *mPtr;              // ptr to flash
  TInt32 mNumResources;      // number of resources
  TUint32 *mResourceTable;    // table of offsets into mROM of resources
  TUint8 *mROM;
  BitmapSlot *mBitmapSlots[MAX_BITMAP_SLOTS];
  RawSlot *mRawSlots[MAX_RAW_SLOTS];
  BBitmap *mPreloadedBitmaps[MAX_PRELOADED_BITMAPS];
};

extern "C" {
extern TUint8 binary_blob_bin_start[];
}
extern BResourceManager gResourceManager;
#endif
