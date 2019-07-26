/**
 * RawFile
 *
 * Reads a file into memory.
 * Can access size, filename, data, etc., once instantiated.
 */
#ifndef MODITE_RAWFILE_H
#define MODITE_RAWFILE_H

#include "rcomp.h"


struct RawFile {
  RawFile(const char *aFilename);

  TUint32 OutputSize();
public:
  TBool   alive;
  char    *filename;
  TUint32 size;
  TUint8  *data;
};


#endif //MODITE_RAWFILE_H
