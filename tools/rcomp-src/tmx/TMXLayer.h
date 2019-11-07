#ifndef RCOMP_TMXLAYER_H
#define RCOMP_TMXLAYER_H

#include "../../src/BTypes.h"

class RawFile;

class TMXLayer {
public:
  TMXLayer(RawFile &aFile, char line[MAX_STRING_LENGTH]);

  ~TMXLayer();

  void Dump();

public:
  TInt id;
  char name[MAX_STRING_LENGTH];
  TUint32 width, height;
  TUint32 *data;
};

#endif //RCOMP_TMXLAYER_H
