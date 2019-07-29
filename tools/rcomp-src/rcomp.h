//
// Created by mschwartz on 7/25/19.
//

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
#include "BTypes.h"
#include "Memory.h"
//
#include "RawFile.h"
#include "BMPFile.h"
#include "ResourceFile.h"
#include "TileMap.h"

#ifndef O_BINARY
#define O_BINARY (0)
#endif

extern void abort(const char *message, ...);
extern void HexDump(TUint8 *ptr, int length);
extern char *skipbl(char *p);
extern char *trim(char *p);
extern void generate_define_name(char *base);


#endif //MODITE_RCOMP_H
