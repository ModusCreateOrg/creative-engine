#ifndef RCOMP_TMXMAP_H
#define RCOMP_TMXMAP_H

#include "../../src/BTypes.h"

class TMXTileSet;

class TMXLayer;

class BObjectProgram;

const TInt MAX_TILES = 4096;

class TMXMap {
public:
  TMXMap(const char *path, const char *filename);

  ~TMXMap();

  void Write(ResourceFile &resourceFile);
  void Dump();

  void ProcessObjects();

protected:
  char *path, *path_buf;
  char *name, *name_buf;
  TInt16 width, height, tileWidth, tileHeight;
  TUint32 *data;
  TUint32 attributes[MAX_TILES];

  TMXTileSet *map_attributes;     // map layer
  TMXTileSet *objects;            // object layer
  TMXTileSet *objects_attributes; // object attributes layer

  TMXLayer *map_layer;
  TMXLayer *map_attributes_layer;
  TMXLayer *objects_layer;
  TMXLayer *objects_attributes_layer;

  TUint16 objectCount;
  BObjectProgram *mObjectProgram;
};

#endif //RCOMP_TMXMAP_H
