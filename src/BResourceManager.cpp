#include "BResourceManager.h"
#include "BBitmap.h"

// TODO: should application incbin the resources binary?

#ifdef __XTENSA__ // ODROID GO TARGET
extern const TUint8 Resources_start[] asm("_binary_Resources_bin_start");
BResourceManager gResourceManager((TAny *)Resources_start);
//extern const TUint8  reesources_bin_start[] asm("_binary_resources_bin_start");
#else // LINUX OR MAC TARGET

#include <stdio.h>


#ifdef __linux__

// LINUX TARGET
asm(
"   .section .rodata\n"
"   .global binary_Resources_bin_start\n"
"   .global binary_Resources_bin_end\n"
"   .balign 16\n"
"binary_Resources_bin_start:\n"
"   .incbin \"Resources.bin\"\n"
"binary_Resources_bin_end:\n"
"   .byte 0\n"
"   .align 4\n"
);
//extern TUint8    binary_Resources_bin_end[];\n"
extern __attribute((aligned(16))) TUint8 binary_Resources_bin_start[];
//extern TUint8    binary_Resources_bin_start[];
//extern TUint8    binary_Resources_bin_end[];
#else

// MAC TARGET
asm(
"   .section .rodata,.rodata\n"
"   .global _binary_Resources_bin_start\n"
"   .global _binary_Resources_bin_end\n"
"   .align 4\n"
"_binary_Resources_bin_start:\n"
"   .incbin \"Resources.bin\"\n"
"_binary_Resources_bin_end:\n"
"   .byte 0\n"
"   .align 4\n"
);
extern "C" {
extern TUint8 binary_Resources_bin_start[];
extern TUint8 binary_Resources_bin_end[];
//extern TUint8 binary_Resources_bin_start[];
}

#endif  // MAC TARGET

BResourceManager gResourceManager(binary_Resources_bin_start);

#endif  // LINUX OR MAC TARGET

struct BitmapSlot {
  BitmapSlot(TInt16 aResourceId, TInt16 aImageType, BBitmap *aBitmap, TBool aCached = EFalse) {
    mResourceId = aResourceId;
    mImageType  = aImageType;
    mBitmap     = aBitmap;
    mCached     = aCached;
  }

  TInt16  mResourceId;
  TInt16  mImageType;
  BBitmap *mBitmap;
  TBool   mCached;
};

struct RawSlot {
  RawSlot(TInt16 aResourceId, BRaw *aRaw, TBool aCached = EFalse) {
    mResourceId = aResourceId;
    mCached     = aCached;
    mRaw        = aRaw;
  }

  TInt16 mResourceId;
  BRaw   *mRaw;
  TBool  mCached;
};

BResourceManager::BResourceManager(TAny *aROM) {
  TUint32 *ptr = (TUint32 *) aROM;
  this->mPtr           = aROM;
  this->mNumResources  = *ptr++;
  this->mResourceTable = ptr;
  this->mROM           = (TUint8 *) &ptr[this->mNumResources];
  for (TInt i = 0; i < MAX_BITMAP_SLOTS; i++) {
    mBitmapSlots[i] = ENull;
  }
  for (TInt i = 0; i < MAX_PRELOADED_BITMAPS; i++) {
    mPreloadedBitmaps[i] = ENull;
  }
  for (TInt i = 0; i < MAX_RAW_SLOTS; i++) {
    mRawSlots[i] = ENull;
  }
//  Dump();
}

BResourceManager::~BResourceManager() {
  ClearBitmapCache();
  ReleaseBitmapSlots();
  ReleaseRawSlots();
}

// Load a bitmap from FLASH/ROM/RODATA into a slot
TBool BResourceManager::LoadBitmap(TInt16 aResourceId, TInt16 aSlotId, TInt16 aImageType) {
  BitmapSlot *slot = mBitmapSlots[aSlotId];
  if (slot) {
    return slot->mResourceId == aResourceId;
  }
  if (mPreloadedBitmaps[aResourceId]) {
    auto *bm = mPreloadedBitmaps[aResourceId];
    mBitmapSlots[aSlotId] = new BitmapSlot(aResourceId, aImageType, bm);
  }
  else {
    auto *bm = new BBitmap(&this->mROM[this->mResourceTable[aResourceId]]);
    mBitmapSlots[aSlotId] = new BitmapSlot(aResourceId, aImageType, bm);
  }
  return ETrue;
}

TBool BResourceManager::PreloadBitmap(TInt16 aResourceId) {
  if (mPreloadedBitmaps[aResourceId]) {
    delete mPreloadedBitmaps[aResourceId];
  }
  auto *bm = new BBitmap(&this->mROM[this->mResourceTable[aResourceId]]);
  mPreloadedBitmaps[aResourceId] = bm;
  return ETrue;
}

TBool BResourceManager::SetBitmap(BBitmap *aBitmap, TInt16 aSlotId, TInt16 aImageType) {
  BitmapSlot *slot = mBitmapSlots[aSlotId];
  if (slot) {
    return slot->mBitmap == aBitmap;
  }
  mBitmapSlots[aSlotId] = new BitmapSlot(-1, aImageType, aBitmap);
  return ETrue;
}

TBool BResourceManager::ReleaseBitmapSlot(TInt16 aSlotId) {
  BitmapSlot *slot = mBitmapSlots[aSlotId];
  if (!slot || slot->mCached) {
#ifndef PRODUCTION
#if (defined(__XTENSA__) && defined(DEBUGME)) || !defined(__XTENSA__)
    printf("ReleaseBitmapSlot failed for %d\n", aSlotId);
#endif
#endif
    return EFalse;
  }
  if (mPreloadedBitmaps[aSlotId] == ENull) {
    // don't delete preloaded bitmaps
    delete slot->mBitmap;
  }
  delete slot;
  mBitmapSlots[aSlotId] = ENull;
  return ETrue;
}

void BResourceManager::ReleaseBitmapSlots() {
  for (TInt16 bm = 0; bm < MAX_BITMAP_SLOTS; bm++) {
    if (mBitmapSlots[bm]) {
      ReleaseBitmapSlot(bm);
    }
  }
}

TBool BResourceManager::CacheBitmapSlot(TInt16 aSlotId, TBool aCacheIt) {
  BitmapSlot *slot = mBitmapSlots[aSlotId];
  if (!slot) {
    return EFalse;
  }
  slot->mCached = aCacheIt;
  return ETrue;
}

void BResourceManager::ClearBitmapCache(TBool aCacheIt) {
  for (TInt16 bm = 0; bm < MAX_BITMAP_SLOTS; bm++) {
    CacheBitmapSlot(bm, aCacheIt);
  }
}

BBitmap *BResourceManager::GetBitmap(TInt16 aSlotId) {
  BitmapSlot *slot = mBitmapSlots[aSlotId];
  if (!slot) {
    return ENull;
  }
  return slot->mBitmap;
}

TInt BResourceManager::BitmapWidth(TInt aSlotId) {
  static const TUint16 widthTable[] = {
    0, 8, 16, 32, 64, 128, 256,
    8, 16, 8, 32, 8, 64, 8, 128, 8, 256,
    16, 32, 16, 64, 16, 128, 16, 256,
    32, 64, 32, 128, 32, 256,
    64, 128, 64, 256,
    128, 256, 256
  };
  BitmapSlot          *slot        = mBitmapSlots[aSlotId];
  if (!slot) {
    return 0;
  }

  return (slot->mImageType == IMAGE_ENTIRE) ? slot->mBitmap->Width() : TInt(widthTable[slot->mImageType]);
}

TInt BResourceManager::BitmapHeight(TInt aSlotId) {
  static const TUint16 heightTable[] = {
    0, 8, 16, 32, 64, 128, 256,
    16, 8, 32, 8, 64, 8, 128, 8, 256, 8,
    32, 16, 64, 16, 128, 16, 256, 16,
    64, 32, 128, 32, 256, 32,
    128, 64, 256, 64,
    128, 256, 256
  };
  BitmapSlot          *slot         = mBitmapSlots[aSlotId];
  if (!slot) {
    return 0;
  }

  return (slot->mImageType == IMAGE_ENTIRE) ? slot->mBitmap->Height() : TInt(heightTable[slot->mImageType]);
}

// BRaw management

TBool BResourceManager::LoadRaw(TInt16 aResourceId, TInt16 aSlotId) {
  RawSlot *slot = mRawSlots[aSlotId];
  if (slot) {
    return slot->mResourceId == aResourceId;
  }
  auto *raw = new BRaw(&this->mROM[this->mResourceTable[aResourceId]]);
  mRawSlots[aSlotId] = new RawSlot(aResourceId, raw);
  return ETrue;
}

TBool BResourceManager::ReleaseRawSlot(TInt16 aSlotId) {
  RawSlot *slot = mRawSlots[aSlotId];
  if (!slot || slot->mCached) {
    return EFalse;
  }
  delete slot->mRaw;
  delete slot;
  mRawSlots[aSlotId] = ENull;
  return ETrue;
}

void BResourceManager::ReleaseRawSlots() {
  for (TInt16 r = 0; r < MAX_RAW_SLOTS; r++) {
    ReleaseRawSlot(r);
  }
}

TBool BResourceManager::CacheRawSlot(TInt16 aSlotId, TBool aCacheIt) {
  RawSlot *slot = mRawSlots[aSlotId];
  if (!slot) {
    return EFalse;
  }
  slot->mCached = aCacheIt;
  return ETrue;
}

void BResourceManager::ClearRawCache(TBool aCacheIt) {
  for (TInt16 r = 0; r < MAX_RAW_SLOTS; r++) {
    CacheRawSlot(r, aCacheIt);
  }
}

BRaw *BResourceManager::GetRaw(TInt16 aSlotId) {
  RawSlot *slot = mRawSlots[aSlotId];
  if (!slot) {
    return ENull;
  }
  return slot->mRaw;
}


void BResourceManager::Dump() {
//#ifndef PRODUCTION
//#if (defined(__XTENSA__) && defined(DEBUGME)) || !defined(__XTENSA__)
  printf("BResourceManager Dump\n");
  printf("mNumResources: %d\n", mNumResources);
  printf("OFFSETS:\n");
  for (TInt i = 0; i < mNumResources; i++) {
    printf("%8d: ", mResourceTable[i]);
    TUint8    *ptr = &mROM[mResourceTable[i]];
    for (TInt j    = 0; j < 16; j++) {
      printf("%02x ", *ptr++);
    }
    printf("\n");
  }
//#endif
//#endif
}
