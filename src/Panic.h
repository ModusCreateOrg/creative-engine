#ifndef PANIC_H
#define PANIC_H

#include "GameEngine.h"

/**
 * Print message and hang
 */
extern void Panic(const char *s);

extern void Dump(TUint32 *ptr, int width, int height=1);
extern void Dump(TUint8 *ptr, int width, int height=1);

#endif

