#include "BTypes.h"
#include "Memory.h"
#include "stdlib.h"

// Global Versions
extern "C" TAny *AllocMem(size_t size, TUint16 aType) {
#ifdef __XTENSA__
  TAny *ptr = heap_caps_malloc(size, aType);
#else
  TAny *ptr = malloc(size);
#endif
//  bzero(ptr, size);
  return ptr;
}

extern "C" TAny *CallocMem(size_t numElements, size_t size, TUint16 aType) {
#ifdef __XTENSA__
  return heap_caps_calloc(numElements, size, aType);
#else
  TAny *ptr = malloc(numElements * size);
  if (ptr == nullptr) {
    return ptr;
  }

  bzero(ptr, numElements * size);
  return ptr;
#endif


}

extern "C" void FreeMem(TAny *ptr) {
  free(ptr);
}

extern "C" TAny *ReallocMem(TAny *aPtr, size_t aSize, TInt16 aType) {
#ifdef __XTENSA__
  return heap_caps_realloc(aPtr, aSize, aType);
#else
  return realloc(aPtr, aSize);
#endif
}

extern "C" TUint32 AvailMem(TUint16 aType) {
#ifdef __XTENSA__
  return (TUint32)heap_caps_get_free_size(aType);
#else
  // we probably don't care :)
  return 4 * 1024 * 1024;
#endif
}

