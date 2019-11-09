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
const int MAX_LEVELS = 10;
const int MAX_STRING_LENGTH = 4096;

//
#include "RawFile.h"
#include "lib/RawBitmap.h"
#include "BMPFile.h"
#include "ResourceFile.h"
#include "TileMap.h"
#include "TMXFile.h"
#include "SpriteSheet.h"

#ifndef O_BINARY
#define O_BINARY (0)
#endif


#include "lib/utils.h"

#endif //MODITE_RCOMP_H
