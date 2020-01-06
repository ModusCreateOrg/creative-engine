#ifndef BMEMORYSTREAM_H
#define BMEMORYSTREAM_H

#include "BBase.h"

/**
  * BMemoryStream
  *
  * Implements a growable raw TUint8 buffer.
  * As yuu Write() to it, it will grow in size to be able to hold all the data you add.
  *
  * The buffer typically is larger than the data it holds, to reduce calling ReallocMem().
  */
const TInt32 STREAM_GROW_SIZE = 4096;

class BMemoryStream : public BBase {
public:
  BMemoryStream(TUint32 aSize = STREAM_GROW_SIZE);
  BMemoryStream(TUint8 *aData, TUint32 aSize, TUint32 aGrowSize = STREAM_GROW_SIZE);
  ~BMemoryStream();

public:
  TInt32 Size() { return mSize; }
  TInt32 ReadOffset() { return mReadIndex; }
  TUint8 *Data() { return mData; }
  void Dump(const char *filename) {
    int fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    write(fd, mData, mSize);
    close(fd);
  }

  

public:
  // Methods to write to stream
  void Write(TAny *aData, TUint32 aSize);

public:
  // methods to read from stream
  void Read(TAny *aData, TUint32 aSize);
  void ReadWithNoFFWD(TAny *aData,  TUint32 aSize, TUint32 aOffset = 0);

  void PrintReadIndex() {   printf("\tBMemoryStream->mReadIndex = %i\n", mReadIndex); }
  void PrintMSize() {   printf("\tBMemoryStream->mSize = %i\n", mSize); }

protected:
  // mSize is the number of bytes added to the buffer via Write(), mAllocated is the buffer's actual size
  TInt32 mSize, mAllocated, mGrowSize, mReadIndex;
  TUint8 *mData;
};

#endif
