#ifndef BTYPES_H
#define BTYPES_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <cmath>

typedef unsigned char  TUint8;
typedef unsigned short TUint16;
typedef unsigned int   TUint32;
typedef unsigned int   TUint;
typedef unsigned long  TUint64;

typedef char  TInt8;
typedef short TInt16;
typedef int   TInt32;
typedef long  TInt64;

typedef int  TInt;
typedef void TAny;
typedef bool TBool;

typedef float TFloat;

#define ETrue (true)
#define EFalse (false)

#define ENull NULL

#ifndef LOBYTEM
#define LOBYTE(x) TUint8((x)&0xff)
#endif

#ifndef HIBYTE
#define HIBYTE(x) TUint8(((x) >> 8) & 0xff)
#endif

#ifndef LOWORD
#define LOWORD(x) TUint16((x)&0xffff)
#endif

#ifndef HIWORD
#define HIWORD(x) TUint16(((x) >> 16) & 0xffff)
#endif

#ifndef ABS
#define ABS(x) ((x) < 0 ? -(x) : (x))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? a : b)
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? a : b)
#endif

#ifndef MID
#define MID(a, b) (((a) + (b)) / 2)
#endif

#define MAXINT (0x7fffffff)

#define ROUND_UP_4(x) (((x) + 3) & -4)

#include "Types/TRGB.h"
#include "Types/TRect.h"
#include "Types/TBCD.h"
#include "Types/TNumber.h"

#endif // BTYPES_H
