#ifdef __MODUS_TARGET_NETWORK_DISPLAY__

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

#include <stdlib.h>
#include <vector>

#include "RemoteMatrixDisplay.h"


//// NETWORKED MATRIX DISPLAY /////

RemoteMatrixDisplay::RemoteMatrixDisplay(const RemoteMatrixDisplayConfig config) {

#ifdef __linux__
  pthread_mutex_destroy(&mMutex);
  pthread_mutex_init(&mMutex, NULL);
#endif
  std::vector<RemoteMatrixSegmentConfig> segmentConfigs = config.segments;

  int mTotalPixels = 0;

  std::vector<RemoteMatrixSegment *> remoteMatrixSegments;


  for (int i = 0; i < segmentConfigs.size(); i++) {
    RemoteMatrixSegmentConfig segmentConfig = segmentConfigs[i];

    mTotalPixels += (segmentConfig.singlePanelWidth * segmentConfig.singlePanelHeight) * segmentConfig.numPanelsWide * segmentConfig.numPanelsTall;

    auto *segment = new RemoteMatrixSegment(segmentConfig);
    remoteMatrixSegments.push_back(segment);
//    segment->Describe();
    segment->StartThread();
//    break;
  }

  mFrameCount = 0;

  mSegments = remoteMatrixSegments;

  mInputBufferSize = config.inputBufferSize;
  mInputBuffer1 = (TUint16 *)malloc(config.inputBufferSize);
  mInputBuffer2 = (TUint16 *)malloc(config.inputBufferSize);
  mCurrentInputBuffer = mInputBuffer1;

  mOutputBufferSize = mTotalPixels * sizeof(TUint16);
  mOutputBuffer1 = (TUint16 *)malloc(mOutputBufferSize);
  mOutputBuffer2 = (TUint16 *)malloc(mOutputBufferSize);
  mCurrentOutputBuffer = mOutputBuffer1;

  mSinglePanelWidth = config.singlePanelWidth;
  mSinglePanelHeight = config.singlePanelHeight;

  mInputScreenWidth = config.inputScreenWidth;
  mInputScreenHeight = config.inputScreenHeight;

  mOutputScreenWidth = config.outputScreenWidth;
  mOutputScreenHeight = config.outputScreenHeight;

}

RemoteMatrixDisplay::~RemoteMatrixDisplay() {
  mThreadRunning = false;
  usleep(100);

  if (mThread.joinable()) {
    mThread.join();
  }

  for (int segmentIdx = 0; segmentIdx < mSegments.size(); segmentIdx++) {
    mSegments[segmentIdx]->StopThread();
  }

  delete mInputBuffer1;
  delete mInputBuffer2;
  delete mOutputBuffer1;
  delete mOutputBuffer2;

}

void RemoteMatrixDisplay::StartThread() {
  mThreadRunning = true;
  mThread = std::thread(&RemoteMatrixDisplay::ThreadFunction, this, this);
  mThread.detach();
}
void RemoteMatrixDisplay::LockMutex() {
  pthread_mutex_lock(&mMutex);
}

void RemoteMatrixDisplay::UnlockMutex() {
  pthread_mutex_unlock(&mMutex);
}

void RemoteMatrixDisplay::ThreadFunction(RemoteMatrixDisplay *remoteDisplay) {
  TUint16 currentFrame = 0;

  while (remoteDisplay->GetThreadRunnning()) {

    if (remoteDisplay->GetFrameCount() == currentFrame) {
//      printf("RemoteMatrixDisplay sleeping...\n");
      usleep(10);
      continue;
    }

    for (int segmentIdx = 0; segmentIdx < remoteDisplay->mSegments.size(); segmentIdx++) {
//    for (int segmentIdx = remoteDisplay->mSegments.size() - 1; segmentIdx > -1 ; segmentIdx--) {
      RemoteMatrixSegment *segment = remoteDisplay->mSegments[segmentIdx];

      segment->LockMutex();

      TUint16 startX = segmentIdx * segment->mSegmentWidth;

      int startingColumn = (mOutputScreenWidth) + (startX);

//      printf("Segment %i : startingColumn = %i\n", segment->mSegmentId, startX);


      for (TUint16 y = 0; y < segment->mSegmentHeight; y++) {
        TUint16 *screenBuffer = &mCurrentOutputBuffer[(y * mOutputScreenWidth) + (startX)];
        TUint16 *segmentBuffer = &segment->GetInputBuffer()[y * segment->mSegmentWidth];

        memcpy(segmentBuffer, screenBuffer, segment->mSegmentWidth * sizeof(TUint16));
      }

      segment->UnlockMutex();
      segment->SwapBuffers();

      segment->IncrementFrameCount();
    }

//    printf("\n-------------------\n");
  }

  printf("RemoteMatrixDisplay::ThreadFunction ended\n");
}

//uint32_t  color = 0;
void RemoteMatrixDisplay::Update() {


  LockMutex();
  memcpy(mCurrentOutputBuffer, mCurrentInputBuffer, mOutputBufferSize);

  mCurrentInputBuffer = (mCurrentInputBuffer == mInputBuffer1) ? mInputBuffer2 : mInputBuffer1;
  mCurrentOutputBuffer = (mCurrentOutputBuffer == mOutputBuffer1) ? mOutputBuffer2 : mOutputBuffer1; // Goes to matrix
  UnlockMutex();



//  LockMutex();
//  UnlockMutex();
  mFrameCount++;

}

void RemoteMatrixDisplay::DescribeSegments() {
  printf("I have %lu segments!\n", mSegments.size());
  for (int i = 0; i < mSegments.size(); i++) {
    mSegments[i]->Describe();
  }
}

TUint16 *RemoteMatrixDisplay::GetInputBuffer() {
  return mCurrentInputBuffer;
}


#endif //#ifdef __MODUS_TARGET_NETWORK_DISPLAY__