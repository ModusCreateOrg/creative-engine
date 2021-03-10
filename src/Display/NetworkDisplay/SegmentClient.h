
#ifndef GENUS_MATRIX_DISPLAY_REMOTEMATRIXSEGMENT_H
#define GENUS_MATRIX_DISPLAY_REMOTEMATRIXSEGMENT_H

#include <thread>

struct SegmentClientConfig {
  uint16_t singlePanelWidth;
  uint16_t singlePanelHeight;
  uint16_t segmentWidth;
  uint16_t segmentHeight;
  uint8_t segmentId;
  char *destinationIP;
  char *destinationPort;
};

class SegmentClient {
public:
  uint8_t  mSegmentId;
  uint16_t mSinglePanelWidth;
  uint16_t mSinglePanelHeight;
  uint16_t mPixelsPerPanel;

  uint16_t mSegmentWidth;
  uint16_t mSegmentHeight;

  uint16_t mTotalPixels;

  size_t mTotalBytes;

  uint16_t *mSegmentBuffer1;
  uint16_t *mSegmentBuffer2;

  char *mDestinationIP;
  char *mDestinationPort;


public:
  explicit SegmentClient(struct SegmentClientConfig config);

  ~SegmentClient();

  void SendDataThread(SegmentClient *mySegment);

  void LockMutex();

  void UnlockMutex();


  void WritePixel(uint16_t index, uint16_t color) {
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

  uint16_t *GetInputBuffer() {
    return mInputBuffer;
  }
  uint16_t *GetOutputBuffer() {
    return mOutputBuffer;
  }

  bool GetThreadRunning() {
    return mThreadRunning;
  }

  void StartThread();
  void StopThread();

  void Describe();

  uint16_t GetFrameCount() {
    return mFrameCount;
  }

  void IncrementFrameCount() {
    mFrameCount++;
//    printf("mySegment.id %i, frame %i\n", mSegmentId, mFrameCount);
  }
private:
  volatile uint16_t mFrameCount;
  volatile bool mThreadRunning;

  uint16_t *mInputBuffer;
  uint16_t *mOutputBuffer;

  pthread_mutex_t mMutex;
  std::thread mThread;

};


#endif //GENUS_MATRIX_DISPLAY_REMOTEMATRIXSEGMENT_H

