/**
 * RawFile
 *
 * Reads a file into memory.
 * Can access size, filename, data, etc., once instantiated.
 */
#ifndef MODITE_RAWFILE_H
#define MODITE_RAWFILE_H

#include "rcomp.h"


class RawFile {
public:
  RawFile(const char *aFilename);
  virtual ~RawFile();
public:
  TUint32 OutputSize();
  char *ReadLine(char *line);
  void Rewind() { input = data;}
public:
  TBool   alive;
  char    *filename;
  TUint32 size;
  TUint8  *data;
  TUint8  *input; // next byte of data for reading
};


#endif //MODITE_RAWFILE_H
