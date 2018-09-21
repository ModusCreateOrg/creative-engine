#include "BBase.h"

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

// Global Versions
TAny *AllocMem(size_t size, TUint16 type) {
#ifdef __XTENSA__
  return heap_caps_malloc(size, type);
#else
  return malloc(size);
#endif
}

void FreeMem(TAny *ptr) { free(ptr); }


TUint32 Milliseconds() {
#ifdef __XTENSA__
  return (TUint32)(esp_timer_get_time() / 1000);
#else
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
#endif
}

#ifndef __XTENSA__
void *operator new(size_t size) { return AllocMem(size, MEMF_SLOW); }
void *operator new[](size_t size) { return AllocMem(size, MEMF_SLOW); }

void operator delete(void *ptr) {
  //
  FreeMem(ptr);
}

void operator delete[](void *ptr) {
  //
  FreeMem(ptr);
}
#endif
