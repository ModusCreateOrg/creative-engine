#ifdef __MODUS_TARGET_DIRECT_LINUX_CONTROLS__

#ifndef DIRECTLINUXCONTROLS_H
#define DIRECTLINUXCONTROLS_H


#include <ControlsBase.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <thread>

#include <linux/input.h>
#include <linux/joystick.h>

#include "axbtnmap.h"


class DirectLinuxControls : public ControlsBase {

public:
  DirectLinuxControls();
  ~DirectLinuxControls();

  TBool Poll() final;

  void PollingThread(DirectLinuxControls *myControls);
  void ParseEvent(struct js_event *event);


  // Is this too much for an int??
  void LockMutex() {
    pthread_mutex_lock(&mMutex);
  }

  void UnlockMutex() {
    pthread_mutex_unlock(&mMutex);
  }

  void StartThread() {
    mThreadRunning = true;
    mThread = std::thread(&DirectLinuxControls::PollingThread, this, this);
    mThread.detach();
  }


  void StopThread() {
    mThreadRunning = false;
    if (mThread.joinable()) {
      mThread.join();
    }
    usleep(100);
  }

private:

  TBool mThreadRunning;
  int fd;
  pthread_mutex_t mMutex;
  std::thread mThread;
};



#endif //DIRECTLINUXCONTROLS_H

#endif // __MODUS_TARGET_DIRECT_LINUX_CONTROLS__