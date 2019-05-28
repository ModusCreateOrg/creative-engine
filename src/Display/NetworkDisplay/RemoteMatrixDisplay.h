#ifdef __MODUS_TARGET_NETWORK_DISPLAY__

#ifndef GENUS_NETWORKED_MATRIX_DISPLAY_MATRIXDISPLAY_H
#define GENUS_NETWORKED_MATRIX_DISPLAY_MATRIXDISPLAY_H
#include <stdint.h>
#include <vector>
#include <thread>

#include "BBase.h"

#include "RemoteMatrixSegment.h"


struct RemoteMatrixDisplayConfig {
  std::vector<RemoteMatrixSegmentConfig> segments;
  size_t inputBufferSize;
  TUint16 inputScreenWidth;
  TUint16 inputScreenHeight;
  TUint16 outputScreenWidth;
  TUint16 outputScreenHeight;
  TUint16 singlePanelHeight;
  TUint16 singlePanelWidth;
  TUint16 totalPanelsWide;
  TUint16 totalPanelsTall;
};

class RemoteMatrixDisplay {

public:

  bool mRunning;

  explicit RemoteMatrixDisplay(const RemoteMatrixDisplayConfig config);
  ~RemoteMatrixDisplay();

  void ThreadFunction(RemoteMatrixDisplay *remoteDisplay);

  void LockMutex();

  void UnlockMutex();
  void DescribeSegments();
  void Update();

  TUint16 *GetInputBuffer();


  void CopyDataToBuffer(TUint16 *buffer);

  void WritePixel(TUint16 index, TUint16 color);
  void StartThread();

  TUint16 GetFrameCount() {
    return mFrameCount;
  }

  TUint16 *mCurrentInputBuffer;

  bool GetThreadRunnning() {
    return mThreadRunning;
  }
  std::thread mThread;

protected:


private:
  size_t mInputBufferSize;
  TUint16 *mInputBuffer1;
  TUint16 *mInputBuffer2;

  size_t mOutputBufferSize;
  TUint16 *mOutputBuffer1;
  TUint16 *mOutputBuffer2;
  TUint16 *mCurrentOutputBuffer;

  TUint16 mOutputScreenWidth;
  TUint16 mOutputScreenHeight;
  TUint16 mInputScreenWidth;
  TUint16 mInputScreenHeight;

  TUint16 mSinglePanelWidth;
  TUint16 mSinglePanelHeight;

  std::vector<RemoteMatrixSegment *> mSegments;
  TUint16 mFrameCount;

  bool mThreadRunning;
  pthread_mutex_t mMutex;
};


#endif //GENUS_MATRIX_DISPLAY_MATRIXDISPLAY_H

#endif //#ifdef __MODUS_TARGET_NETWORK_DISPLAY__