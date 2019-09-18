#ifndef RCOMP_TILEMAP_H
#define RCOMP_TILEMAP_H

#include "rcomp.h"

const int MAX_LEVELS = 10;

class RawFile;

class BMPFile;

struct LayerInfo {
  TBool used;
  RawFile *map;                   // map layer
  RawFile *map_attributes;        // map attributes layer
  RawFile *object;                // object layer
  RawFile *object_attributes;     // object attributes layer
};

class TileMap {
public:
  TileMap(const char *path, const char *filename);

  ~TileMap();

public:
  void Write(ResourceFile &resourceFile);

public:
  const char *filename;
  BMPFile    *bmp;
  LayerInfo  mLevels[MAX_LEVELS];
  RawFile    *mapAttributes;   // .tlc file contents = num_tiles_in_bmp words
};

#endif //RCOMP_TILEMAP_H
