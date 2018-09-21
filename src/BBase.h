#ifndef BBASE_H
#define BBASE_H

#include "BTypes.h"

/**
 * Base class for EVERYTHING.
 */

#ifdef __XTENSA__
#include "freertos/FreeRTOS.h"
#define MEMF_SLOW (MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM)
#define MEMF_FAST (MALLOC_CAP_8BIT | MALLOC_CAP_DMA)

#else 

#define MEMF_SLOW 0
#define MEMF_FAST (1 << 0)

#endif

class BBase {
public:
  BBase();
  ~BBase();
};

extern TAny *AllocMem(size_t size, TUint16 flags);
extern void FreeMem(TAny *ptr);

#ifdef __XTENSA__
inline void *operator new(size_t size) { return AllocMem(size, MEMF_SLOW); }

inline void *operator new[](size_t size) { return AllocMem(size, MEMF_SLOW); }

inline void operator delete(void *ptr) {
  //
  FreeMem(ptr);
}

inline void operator delete[](void *ptr) {
  //
  FreeMem(ptr);
}
#else
// Cannot inline on the host (defined in BBase.cpp)
extern void *operator new(size_t size);
extern void *operator new[](size_t size);

extern void operator delete(void *ptr);
extern void operator delete[](void *ptr);
#endif

extern TUint32 Milliseconds();

#endif
