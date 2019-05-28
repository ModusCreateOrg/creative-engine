#ifdef __MODUS_TARGET_NETWORK_DISPLAY__

#ifndef GENUS_MATRIX_DISPLAY_REMOTEMATRIXSEGMENT_H
#define GENUS_MATRIX_DISPLAY_REMOTEMATRIXSEGMENT_H

#include <thread>
#include "BBase.h"

struct RemoteMatrixSegmentConfig {
  TUint16 singlePanelWidth;
  TUint16 singlePanelHeight;
  TUint8 numPanelsWide;
  TUint8 numPanelsTall;
  TUint8 segmentId;
  char *destinationIP;
  char *destinationPort;
};

class RemoteMatrixSegment {
public:
  TUint8  mSegmentId;
  TUint16 mSinglePanelWidth;
  TUint16 mSinglePanelHeight;
  TUint16 mPixelsPerPanel;

  TUint16 mPanelsWide;
  TUint16 mPanelsTall;

  TUint16 mSegmentWidth;
  TUint16 mSegmentHeight;

  TUint16 mTotalPixels;

  size_t mTotalBytes;

  TUint16 *mSegmentBuffer1;
  TUint16 *mSegmentBuffer2;

  char *mDestinationIP;
  char *mDestinationPort;


public:
  explicit RemoteMatrixSegment(struct RemoteMatrixSegmentConfig config);

  ~RemoteMatrixSegment();

  void SendDataThread(RemoteMatrixSegment *mySegment);

  void LockMutex();

  void UnlockMutex();


  void WritePixel(TUint16 index, TUint16 color) {
    LockMutex();
    mInputBuffer[index] = color;
    UnlockMutex();
  }

  void SwapBuffers() {
    LockMutex();

    if (mInputBuffer == mSegmentBuffer1) {
      mInputBuffer = mSegmentBuffer2;
      mOutputBuffer = mSegmentBuffer1;
    }
    else {
      mInputBuffer = mSegmentBuffer1;
      mOutputBuffer = mSegmentBuffer2;
    }

    UnlockMutex();
  }

  TUint16 *GetInputBuffer() {
    return mInputBuffer;
  }
  TUint16 *GetOutputBuffer() {
    return mOutputBuffer;
  }

  bool GetThreadRunning() {
    return mThreadRunning;
  }

  void StartThread();
  void StopThread();

  void Describe();

  TUint16 GetFrameCount() {
    return mFrameCount;
  }

  void IncrementFrameCount() {
    mFrameCount++;
//    printf("mySegment.id %i, frame %i\n", mSegmentId, mFrameCount);
  }
private:
  volatile TUint16 mFrameCount;
  volatile bool mThreadRunning;

  TUint16 *mInputBuffer;
  TUint16 *mOutputBuffer;

  pthread_mutex_t mMutex;
  std::thread mThread;
};


#endif //GENUS_MATRIX_DISPLAY_REMOTEMATRIXSEGMENT_H

#endif //#ifdef __MODUS_TARGET_NETWORK_DISPLAY__