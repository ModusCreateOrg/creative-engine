#include "RawFile.h"
#include "TileMap.h"

TileMap::TileMap(const char *filename) {
  this->filename = strdup(filename);

  char work[2048], resourceFn[2048];

  RawFile txt(filename);
  if (!txt.alive) {
    abort("Can't open %s for reading\n", filename);
  }
  while (txt.ReadLine(work)) {
    //
    char *ptr = strrchr(work, '\\');
    if (ptr == ENull) {
      ptr = work;
    }
    else {
      ptr++;
    }

    printf("-> %s\n", ptr);
    sprintf(resourceFn, "%s/%s", resourceFile.path, ptr);
    const char *extension = &ptr[strlen(ptr)-3];
    if (!strcasecmp(extension, "bmp")) {
      this->bmp = new BMPFile(resourceFn);
      printf("BITMAP %s\n", ptr);
    }
    else if (!strcasecmp(extension, "tlc")) {
      printf("TLC %s\n", ptr);
      this->mapAttributes = new RawFile(resourceFn);
    }
    else if (!strcasecmp(extension, "stm")) {
      printf("STM %s\n", ptr);
      if (strcasestr(ptr, "TILE_LAYER") != ENull) {
        this->mapData = new RawFile(resourceFn);
      }
      else {
        printf("-> IGNORING %s (for now)\n", ptr);
      }
    }
    else {
      abort("unknown extension: %s\n", ptr);
    }
  }
}

TileMap::~TileMap() {
  //
  delete this->filename;
}

struct MapData {
  char token[4];
  TUint16 width, height;
  TUint32 data[];
};

void TileMap::Write(ResourceFile &resourceFile) {
  TUint16 *tlc = (TUint16 *)mapAttributes->data;
  MapData *map = (MapData *)mapData->data;
  TUint32 *data = &map->data[0];
  // We set the attributes bits in the map so the game can fiddle the individual tiles' bits during play.
  for (TInt n=0; n<map->width * map->height; n++) {
    data[n] |= TUint32(tlc[data[n]]) << 16;
  }
}
