#ifndef MODITE_RCOMP_H
#define MODITE_RCOMP_H

#include <cstdio>
#include <sys/types.h>
#include <cstdarg>
#include <cstdint>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <libgen.h>
//
#include "../../src/BTypes.h"
#include "../../src/Memory.h"
#include "../../src/Panic.h"
#include "../../src/BBitmap.h"
//
#include "RawFile.h"
#include "RawBitmap.h"
#include "BMPFile.h"
#include "ResourceFile.h"
#include "TileMap.h"
#include "TMX.h"
#include "SpriteSheet.h"

#ifndef O_BINARY
#define O_BINARY (0)
#endif

extern void HexDump(TUint8 *ptr, TInt length, TInt width=8);
extern void HexDump(TUint16 *ptr, TInt length, TInt width=8);
extern void HexDump(TUint32 *ptr, TInt length, TInt width=8);
extern char *skipbl(char *p);
extern char *parse_token(char *dst, char *src);
extern char *trim(char *p);
extern void generate_define_name(char *base);

#endif //MODITE_RCOMP_H
