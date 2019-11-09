#ifndef RCOMP_TILEMAP_H
#define RCOMP_TILEMAP_H

//#include "rcomp.h"
//#include "LayerInfo.h"
//#include "TileMapBase.h"

class LayerInfo;
class BMPFile;
class RawFile;

class TileMap {
public:
  TileMap(const char *path, const char *filename);

  ~TileMap();

public:
  void Write(ResourceFile &resourceFile);

public:
  const char *filename;
  BMPFile *bmp;
  LayerInfo *mLevels[MAX_LEVELS];
  RawFile *mapAttributes;   // .tlc file contents = num_tiles_in_bmp words
};

#endif //RCOMP_TILEMAP_H
