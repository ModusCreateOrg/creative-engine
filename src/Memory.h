#ifndef CREATIVE_ENGINE_MEMORY_H
#define CREATIVE_ENGINE_MEMORY_H

#ifdef __cplusplus
/**
 * Allocate memory of specified size and type
 *
 * @param aSize
 * @param aFlags MEMF_FAST or MEMF_SLOW
 * @return
 */
extern "C" TAny *AllocMem(size_t aSize, TUint16 aFlags);

/**
 * Free memory allocated with AllocMem
 * @param ptr
 */
extern "C" void FreeMem(TAny *ptr);

/**
 * Resize memory allocated with AllocMem
 */
extern "C" TAny *ReallocMem(TAny *aPtr, size_t aSize, TInt16 aType);

/**
 * Return available memory of specified type
 */
extern "C" TUint32 AvailMem(TUint16 aFlags);

#else
#include <stddef.h>
typedef void *TAny;
typedef unsigned short TUint16;
typedef short TInt16;
typedef unsigned int TUint32;
#define EFalse (0)
#define ETrue (!0)
typedef int TBool;

/**
 * Allocate memory of specified size and type
 *
 * @param aSize
 * @param aFlags MEMF_FAST or MEMF_SLOW
 * @return
 */
extern  TAny *AllocMem(size_t aSize, TUint16 aType);
extern  TAny *CallocMem(size_t numElements, size_t aSize, TUint16 aType);
/**
 * Free memory allocated with AllocMem
 * @param ptr
 */
extern  void FreeMem(TAny *ptr);

/**
 * Resize memory allocated with AllocMem
 */
extern  TAny *ReallocMem(TAny *aPtr, size_t aSize, TInt16 aType);

/**
 * Return available memory of specified type
 */
extern  TUint32 AvailMem(TUint16 aFlags);
#endif

#ifdef __XTENSA__
#include "freertos/FreeRTOS.h"
#define MEMF_SLOW (MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM)
#define MEMF_FAST (MALLOC_CAP_8BIT | MALLOC_CAP_DMA)

#else

#define MEMF_SLOW 0
#define MEMF_FAST (1 << 0)

#endif

#endif //CREATIVE_ENGINE_MEMORY_H
