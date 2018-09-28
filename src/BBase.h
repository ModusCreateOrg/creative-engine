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

//////// RANDOM NUMBER GENERATOR

/**
 * SeedRandom
 *
 * Seed random number generator.
 *
 * @param aSeed
 */
void SeedRandom(TUint32 aSeed);

/**
 * Generate a random number
 * @return
 */
TUint32 Random();

/**
 * Generate a random number in given range.
 *
 * Note: this routine produces signed result and takes signed arguments.  This allows a range of something like -10 to 100.
 *
 * @param aMin
 * @param aMax
 * @return
 */
TInt32 Random(TInt32 aMin, TInt32 aMax);

/**
 * Return random number between 0 and 1 (floating point)
 * @return
 */
TFloat RandomFloat();

/**
 * Allocate memory of specified size and type
 *
 * @param aSize
 * @param aFlags MEMF_FAST or MEMF_SLOW
 * @return
 */
extern TAny *AllocMem(size_t aSize, TUint16 aFlags);

/**
 * Free memory allocated with AllocMem
 * @param ptr
 */
extern void FreeMem(TAny *ptr);

/**
 * Resize memory allocated with AllocMem
 */
extern TAny *ReallocMem(TAny *ptr, size_t aSize);

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
