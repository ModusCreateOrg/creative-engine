#ifndef RCOMP_TMX_H
#define RCOMP_TMX_H

class BMPFile;

#include "LayerInfo.h"

class TMX {
public:
  TMX(const char *path, char *filename);
  ~TMX();
public:
  const char *filename;
  BMPFile *bmp;
  LayerInfo *mLevels[MAX_LEVELS];
};


#endif //RCOMP_TMX_H
