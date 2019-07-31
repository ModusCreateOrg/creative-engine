#include "RawFile.h"
#include "TileMap.h"

TileMap::TileMap(const char *path, const char *filename) {
  this->filename = strdup(filename);
  this->mapAttributes = ENull;
  this->mapData = ENull;
  this->bmp = ENull;

  char work[2048], resourceFn[2048];
  sprintf(work, "%s/%s", path, filename);

  RawFile txt(work);
  if (!txt.alive) {
    abort("Can't open %s for reading\n", work);
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
  if (!mapAttributes) {
    abort("TileMap (%s) missing TLC\n:", filename);
  }
  if (!mapData) {
    abort("TileMap (%s) missing STM\n:", filename);
  }
  if (!bmp) {
    abort("TileMap (%s) missing BMP\n:", filename);
  }

  // write out the BMP
  char work[2048];
  sprintf(work, "%s_BMP", filename);
  TUint16 bmp_id = resourceFile.StartResource(work);
  bmp->Write(resourceFile);

  TUint16 *tlc = (TUint16 *)mapAttributes->data;
  MapData *map = (MapData *)mapData->data;
  TUint32 *data = &map->data[0];
  printf("TILEMAP %s is %dx%d\n", filename, map->width, map->height);
  HexDump(data, 16);
  // We set the attributes bits in the map so the game can fiddle the individual tiles' bits during play.
  for (TInt n=0; n<map->width * map->height; n++) {
    data[n] |= TUint32(tlc[data[n]]) << 16;
  }
  HexDump(data, 16);

  sprintf(work, "%s_MAP", filename);
  resourceFile.StartResource(work);
  resourceFile.Write(&bmp_id, sizeof(bmp_id));

  resourceFile.Write(&map->width, sizeof(map->width));
  resourceFile.Write(&map->height, sizeof(map->height));
  resourceFile.Write(&data[0], map->width * map->height * sizeof(TUint32));
}
