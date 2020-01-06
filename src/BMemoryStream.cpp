#include "BMemoryStream.h"
#include "Panic.h"

BMemoryStream::BMemoryStream(TUint32 aGrowSize) {
  mGrowSize = aGrowSize;
  mSize = 0;
  mData = (TUint8 *)AllocMem(mGrowSize, MEMF_SLOW);
  mAllocated = aGrowSize;
  mReadIndex = 0;
}

BMemoryStream::BMemoryStream(TUint8 *aData, TUint32 aSize, TUint32 aGrowSize) {
  mData = new TUint8[aSize];
  mSize = aSize;
  memcpy(mData, aData, mSize);
  mGrowSize = aGrowSize;
  mAllocated = aSize;
  mGrowSize = aGrowSize;
  mReadIndex = 0;
}

BMemoryStream::~BMemoryStream() {
  FreeMem(mData);
  mData = ENull;
  mSize = 0;
}

void BMemoryStream::Write(TAny *aData, TUint32 aSize) {
  TUint8 *data = (TUint8 *)aData;
  TUint32 new_size = mSize + aSize;

  // grow buffer is data doesn't fit in the free space
  if (new_size > mAllocated) {
    mData = (TUint8 *)ReallocMem(mData, new_size + STREAM_GROW_SIZE, MEMF_SLOW);
    mAllocated = new_size + mGrowSize;
  }

  memcpy(&mData[mSize], (TUint8 *)aData, aSize);
  mSize = new_size;
}

void BMemoryStream::Read(TAny *aData, TUint32 aSize) {
  if (mReadIndex > mSize) {
    Panic("BMemoryStream::Read beyond end of stream\n");
  }
  TUint8 *dst = (TUint8 *)aData;
  memcpy(dst, &mData[mReadIndex], aSize);
  mReadIndex += aSize;
}


void BMemoryStream::ReadWithNoFFWD(TAny *aData, TUint32 aSize, TUint32 aOffset) {
  if (mReadIndex > mSize) {
    Panic("BMemoryStream::Read beyond end of stream\n");
  }
  TUint8 *dst = (TUint8 *)aData;
  memcpy(dst, &mData[mReadIndex + aOffset], aSize);
}

