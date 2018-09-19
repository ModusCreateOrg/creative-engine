#include "GameEngine.h"

#ifndef __XTENSA__

#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <math.h>

#else

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#endif

BBase::BBase() {
  //
}

BBase::~BBase() {
  //
}

#if 0
TAny *BBase::AllocMem(size_t size, TInt32 type) {
#ifndef __XTENSA__
  return malloc(size);
#else
  return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
#endif
}

void BBase::FreeMem(TAny *ptr) {
  //
  free(ptr);
}


void *BBase::operator new(size_t size) { return ::AllocMem(size, MEMF_SLOW); }

void *BBase::operator new[](size_t size) { return ::AllocMem(size, MEMF_SLOW); }

void BBase::operator delete(void *ptr) {
  //
  ::FreeMem(ptr);
}

void BBase::operator delete[](void *ptr) {
  //
  ::FreeMem(ptr);
}
#endif

// Global Versions
TAny *AllocMem(size_t size, TInt32 type) {
#ifndef __XTENSA__
  return malloc(size);
#else
  return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
#endif
}

void FreeMem(TAny *ptr) { free(ptr); }


TUint32 Milliseconds() {
#ifndef __XTENSA__
  TUint32         ms;
  time_t          s;
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
#else
  return (TUint32)(esp_timer_get_time() / 1000);
#endif
}
