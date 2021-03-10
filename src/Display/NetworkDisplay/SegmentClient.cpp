
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>

#include "SegmentClient.h"


using boost::asio::ip::tcp;


SegmentClient::SegmentClient(struct SegmentClientConfig config) {
#ifdef __linux__
  pthread_mutex_destroy(&mMutex);
  pthread_mutex_init(&mMutex, NULL);
#endif

  mFrameCount = 0;
  mSegmentId = config.segmentId;
  mSinglePanelWidth = config.singlePanelWidth;
  mSinglePanelHeight = config.singlePanelHeight;
  mPixelsPerPanel = mSinglePanelWidth * mSinglePanelWidth;

  mSegmentWidth = config.segmentWidth;
  mSegmentHeight = config.segmentHeight;

  mTotalPixels = mSegmentWidth * mSegmentHeight;
  mTotalBytes = mTotalPixels * sizeof(uint16_t);


  mSegmentBuffer1 = (uint16_t *)malloc(mTotalBytes);
  mSegmentBuffer2 = (uint16_t *)malloc(mTotalBytes);

  mInputBuffer = mSegmentBuffer1;

  mDestinationIP = (char *)malloc(sizeof(config.destinationIP));
  strcpy(mDestinationIP, config.destinationIP);

  mDestinationPort = (char *)malloc(sizeof(config.destinationPort));
  strcpy(mDestinationPort, config.destinationPort);

  printf("SegmentClient %s (id = %i)\n", mDestinationIP, mSegmentId);
}



void SegmentClient::SendDataThread(SegmentClient *mySegment) {

  uint16_t currentFrame = 0;

  auto *data = (uint16_t *)malloc(mySegment->mTotalBytes);

//  uint16_t color = 0;

  while (mySegment->GetThreadRunning()) {

    if (mySegment->GetFrameCount() == currentFrame) {
      usleep(50);
      continue;
    }

    try {

      boost::asio::io_service io_service;

      tcp::socket s(io_service);
      tcp::resolver resolver(io_service);

      boost::asio::connect(s, resolver.resolve({mySegment->mDestinationIP, mySegment->mDestinationPort}));

      mySegment->LockMutex();
      uint16_t *sBuffPtr = mySegment->GetOutputBuffer();
//      memset(sBuffPtr, ++color, mySegment->mTotalBytes);
      memcpy(data, sBuffPtr, mySegment->mTotalBytes);
      mySegment->UnlockMutex();

      size_t numBytesWritten = boost::asio::write(s, boost::asio::buffer(data, mySegment->mTotalBytes));

      char reply[10];
      size_t reply_length = boost::asio::read(s,boost::asio::buffer(reply, 1));

      if (reply_length != 1) {
        printf("* BAD *\n\n");
      }

      currentFrame = mySegment->GetFrameCount();

    }
    catch (std::exception& e) {
      std::cerr << mySegment->mDestinationIP << " " <<  __FUNCTION__ << " Exception: " << e.what() << "\n";
    }
  }

  delete data;
  printf("SegmentClient::SendDataThread ended %i\n", mySegment->mSegmentId);
}



void SegmentClient::StartThread() {
  mThreadRunning = true;
  mThread = std::thread(&SegmentClient::SendDataThread, this, this);
  mThread.detach();
}


void SegmentClient::StopThread() {
  mThreadRunning = false;
  if (mThread.joinable()) {
    mThread.join();
  }
  usleep(100);
}

void SegmentClient::LockMutex() {
  pthread_mutex_lock(&mMutex);
}

void SegmentClient::UnlockMutex() {
  pthread_mutex_unlock(&mMutex);
}

void SegmentClient::Describe() {
  printf("SegmentClient %p\n", this);
  printf("\tmSegmentId = %i\n", mSegmentId);
  printf("\tmSinglePanelWidth = %i\n", mSinglePanelWidth);
  printf("\tmSinglePanelHeight = %i\n", mSinglePanelHeight);
  printf("\tmPixelsPerPanel = %i\n", mPixelsPerPanel);
  printf("\tmSegmentWidth = %i\n", mSegmentWidth);
  printf("\tmSegmentHeight = %i\n", mSegmentHeight);
  printf("\tmTotalPixels = %i\n", mTotalPixels);
  printf("\tmTotalBytes = %lu\n", mTotalBytes);
  printf("\tmDestinationIP = %s\n", mDestinationIP);
  printf("\tmDestinationPort = %s\n", mDestinationPort);
  fflush(stdout);
}

SegmentClient::~SegmentClient() {
  StopThread();


  delete mSegmentBuffer1;
  delete mSegmentBuffer2;
}

