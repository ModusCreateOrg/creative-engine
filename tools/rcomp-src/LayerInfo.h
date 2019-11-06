#ifndef RCOMP_LAYERINFO_H
#define RCOMP_LAYERINFO_H

#include "../../src/BTypes.h"

const int MAX_LEVELS = 10;

class RawFile;

struct LayerInfo {
  TBool used;
  RawFile *map;                   // map layer
  RawFile *map_attributes;        // map attributes layer
  RawFile *object;                // object layer
  RawFile *object_attributes;     // object attributes layer
};

#endif //RCOMP_LAYERINFO_H
