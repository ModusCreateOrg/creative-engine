#ifndef GENUS_NETWORKED_MATRIX_DISPLAY_MATRIXDISPLAY_H
#define GENUS_NETWORKED_MATRIX_DISPLAY_MATRIXDISPLAY_H
#include <stdint.h>
#include <vector>
#include <thread>
#include <time.h>
#include <cmath>
#include <unistd.h>

#include "SegmentClient.h"
#include "DisplayDefines.h"
#include "Display.h"

#include "NetworkDisplayConfig.h"

class NetworkDisplay  : public Display {
public:
  static NetworkDisplayConfig GenerateConfig(char *aFile) {
    NetworkDisplayConfig displayConfig;

    int error = ini_parse(aFile, ini_file_handler, &displayConfig);
    if (error != 0) {
      fprintf(stderr, "Fatal Error: Can't parse %s. Error code %i.\n", aFile, error);
      fflush(stderr);
      exit(error);
    }


    return displayConfig;
  }

  NetworkDisplay();
  ~NetworkDisplay();
  void Init() override {};

  TUint16 color565(TUint8 red, TUint8 green, TUint8 blue) override{
    // lifted from Display2.cpp
    // maybe we want to return something else for SDL
    return ((blue & 0xF8) << 8) | ((red & 0xFC) << 3) | ((green & 0xF8) >> 3);
  }
  bool ConfigureFromIniFile(char *fileName);

  void ThreadFunction(NetworkDisplay *remoteDisplay);

  void DescribeSegments();
  void Update() override;

  uint16_t *GetInputBuffer();

//  void WritePixel(uint16_t index, uint16_t color);

  void SwapBuffersNetwork() {
    LockMutex();
    mCurrInBuffer = (mCurrInBuffer == mInputBuffer1) ? mInputBuffer2 : mInputBuffer1;
    mCurrOutBuffer = (mCurrOutBuffer == mOutputBuffer1) ? mOutputBuffer2 : mOutputBuffer1; // Goes to matrix
    UnlockMutex();
  }

  uint16_t GetFrameCount() {
    return mFrameCount;
  }

  void Clear(uint16_t aColor = 0) {
    memset(GetInputBuffer(), GetInputBufferSize(), aColor);
  }

public:

  uint16_t GetOutputScreenWidth() {
    return mOutputScreenWidth;
  }
  uint16_t GetOutputScreenHeight() {
    return mOutputScreenHeight;
  }
  uint16_t GetInputScreenWidth() {
    return mInputScreenWidth;
  }
  uint16_t GetInputScreenHeight() {
    return mInputScreenHeight;
  }
  size_t GetTotalInputPixels() {
    return mInputBufferSize;
  }

  size_t GetTotalOutputPixels() {
    return mOutputBufferSize;
  }

  bool GetThreadRunnning() {
    return mThreadRunning;
  }


  void StartThread() {
    mThreadRunning = true;
    mThread = std::thread(&NetworkDisplay::ThreadFunction, this, this);
    mThread.detach();
  }

  void LockMutex() {
    pthread_mutex_lock(&mMutex);
  }

  void UnlockMutex() {
    pthread_mutex_unlock(&mMutex);
  }

  size_t GetInputBufferSize() {
    return mInputBufferSize;
  }

public:
  uint32_t mSNow;
  uint32_t mSNext;

//  void NextFrameDelay() {
//    if (mFrameRate < 0) {
//      return;
//    }
//
//    mSNext = (mSNext + 1000 / mFrameRate);
//
//
//    if (mSNow < mSNext) {
//      uint16_t sleepTime = (mSNext - mSNow) * 1000;
//      usleep(sleepTime);
//      mSNow = Milliseconds();
//    }
//
//  }

  static uint32_t Milliseconds() {
    uint32_t ms;
    time_t s;
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);
    s  = spec.tv_sec;
    ms = lround(spec.tv_nsec / 1.0e6);
    if (ms > 999) {
      s++;
      ms = 0;
    }
    ms += s * 1000;
    return ms;
  }


private:
  NetworkDisplayConfig mConfig{};

  int mFrameRate;

  void InitNetworkSegments();

  std::thread mThread;



  size_t mInputBufferSize;
  uint32_t mTotalInputPixels;
  uint16_t *mCurrInBuffer;
  uint16_t *mInputBuffer1;
  uint16_t *mInputBuffer2;

  size_t mOutputBufferSize;
  uint32_t mTotalOutputPixels;
  uint16_t *mCurrOutBuffer;
  uint16_t *mOutputBuffer1;
  uint16_t *mOutputBuffer2;

  uint16_t mOutputScreenWidth;
  uint16_t mOutputScreenHeight;
  uint16_t mInputScreenWidth;
  uint16_t mInputScreenHeight;

  uint16_t mSinglePanelWidth;
  uint16_t mSinglePanelHeight;

  std::vector<SegmentClient *> mSegments;
  uint16_t mFrameCount;

  bool mThreadRunning{};
  pthread_mutex_t mMutex{};

#ifdef __USE_SDL2_VIDEO__
  SDL2Display *mSDL2Display;
#endif
};


#endif //GENUS_MATRIX_DISPLAY_MATRIXDISPLAY_H

