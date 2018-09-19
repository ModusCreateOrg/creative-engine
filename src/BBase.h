#ifndef BBASE_H
#define BBASE_H

#include "BTypes.h"

/**
 * Base class for EVERYTHING.
 */

#define MEMF_SLOW 0
#define MEMF_FAST (1 << 0)

class BBase {
public:
  BBase();
  ~BBase();

public:
  TAny *AllocMem(size_t size, TInt32 flags);
  void FreeMem(TAny *ptr);

};

extern TAny *AllocMem(size_t size, TInt32 flags);
extern void FreeMem(TAny *ptr);
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

extern TUint32 Milliseconds();

#endif
