#ifndef BBASE_H
#define BBASE_H

// Uncomment for production build
#define PRODUCTION

#include "BTypes.h"
#include "Memory.h"

/**
 * BBase is the base class for EVERYTHING.
 */
class BBase {
public:
  BBase();

  ~BBase();
};

//////// RANDOM NUMBER GENERATOR

/**
 * \brief SeedRandom
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
