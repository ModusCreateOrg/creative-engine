#include "BMemoryStream.h"
#include "Panic.h"

BMemoryStream::BMemoryStream(TUint32 aGrowSize) {
  mGrowSize = aGrowSize;
  mData = (TUint8 *)AllocMem(mGrowSize, MEMF_SLOW);
  mAllocated = aGrowSize;
  mReadIndex = 0;
  mFreeMe = ETrue;
}

BMemoryStream::BMemoryStream(TUint8 *aData, TUint32 aSize, TUint32 aGrowSize) {
  mData = aData;
  mSize = aSize;
  mGrowSize = aGrowSize;
  mAllocated = aSize;
  mGrowSize = aGrowSize;
  mFreeMe = EFalse;
  mReadIndex = 0;
}

BMemoryStream::~BMemoryStream() {
  if (mFreeMe) {
    FreeMem(mData);
  }
  mData = ENull;
  mSize = 0;
}

void BMemoryStream::Write(TAny *aData, TUint32 aSize) {
  if (!mFreeMe) {
    Panic("BMemoryStream::Write to read only stream!");
  }

  TUint8 *data = (TUint8 *)aData;
  TUint32 new_size = mSize + aSize;

  // grow buffer is data doesn't fit in the free space
  if (new_size > mAllocated) {
    mData = (TUint8 *)ReallocMem(mData, new_size + STREAM_GROW_SIZE, MEMF_SLOW);
    mAllocated = new_size + mGrowSize;
  }

  memcpy(&mData[mSize], aData, aSize);
  mSize += aSize;
}

void BMemoryStream::Read(TAny *aData, TUint32 aSize) {
  if (mReadIndex > mSize) {
    Panic("BMemoryStream::Read beyond end of stream\n");
  }
  memcpy(aData, &mData[mReadIndex], aSize);
  mReadIndex += aSize;
}

