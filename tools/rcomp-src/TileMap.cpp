#include "RawFile.h"
#include "TileMap.h"

#define DEBUGME
#undef DEBUGME

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
    printf("Processing %s\n", work);
    if (!strlen(work)) {
      continue;
    }
    //
    char *ptr = strrchr(work, '\\');
    if (ptr == ENull) {
      ptr = work;
    } else {
      ptr++;
    }

    sprintf(resourceFn, "%s/%s", resourceFile.path, ptr);
    const char *extension = &ptr[strlen(ptr) - 3];
    if (!strcasecmp(extension, "bmp")) {
      printf("-> %s\n", ptr);
      this->bmp = new BMPFile(resourceFn);
//      printf("BITMAP %s\n", ptr);
    } else if (!strcasecmp(extension, "tlc")) {
      printf("-> %s\n", ptr);
      this->mapAttributes = new RawFile(resourceFn);
    } else if (!strcasecmp(extension, "stm")) {
      if (strcasestr(ptr, "MAP_LAYER") != ENull) {
        printf("-> %s\n", ptr);
        this->mapData = new RawFile(resourceFn);
      } else if (strcasestr(ptr, "DATA_LAYER")) {
        printf("-> OBJECT DATA %s\n", ptr);
        this->objectData = new RawFile(resourceFn);
      } else {
        printf("-> %s IGNORED (for now)\n", ptr);
      }
    } else {
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

struct ObjectLayer {
  ObjectLayer(TInt32 aWidth, TInt32 aHeight, TUint32 *aData, TUint32 size) {
    width = aWidth;
    height = aHeight;

    data = new TUint32[size];
    bzero(data, size);
    memcpy(data, aData, size);
  }

  TUint16 GetCell(TInt row, TInt col) {
    return data[row * width + col];
  }

  void SetCell(TInt row, TInt col, TUint16 val) {
    data[row * width + col] = val;
  }

  void Dump(TInt row) {
    printf("ROW %03d ", row);
    HexDump(&data[row * width], width, width);
  }

  void Dump() {
    for (TInt row = 0; row < height; row++) {
      Dump(row);
    }
    printf("\n");
  }

  TInt32 width, height;
  TUint32 *data;
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

  TUint16 *tlc = (TUint16 *) mapAttributes->data;
  MapData *map = (MapData *) mapData->data;
  TInt32 width = map->width,
    height = map->height;

  TUint32 *data = &map->data[0];
  printf("--> TILEMAP %s is %dx%d\n", filename, map->width, map->height);
  // We set the attributes bits in the map so the game can fiddle the individual tiles' bits during play.
  for (TInt n = 0; n < map->width * map->height; n++) {
    data[n] |= tlc[data[n]] << TUint16(16);
  }

  ObjectLayer objectLayer(width, height, (TUint32 *) &objectData->data[4 * sizeof(TUint16)],
                          objectData->size); // skip over STM and width,height

  // count objects in DATA_LAYER
  TUint16 objectCount = 0;
#ifdef DEBUGME
  objectLayer.Dump();
#endif
  for (TInt row = 0; row < height; row++) {
    for (TInt col = 0; col < width; col++) {
      TUint32 tile = objectLayer.GetCell(row, col) & TUint32(0xffff);
      TUint16 attr = tlc[tile];
      if (tile != 0 && attr != 0) {
//        printf("Found %d at row,col %d,%d\n", attr, row, col);
        // zero out tile to right and two tiles below so we don't process them
//        objectLayer.SetCell(row, col, 0);
        objectLayer.SetCell(row, col + 1, 0);
        objectLayer.SetCell(row + 1, col, 0);
        objectLayer.SetCell(row + 1, col + 1, 0);
        objectCount++;
      }
    }
  }
#ifdef DEBUGME
  objectLayer.Dump();
#endif

  // generate OBJECT program
//  printf("Found %d objects\n", objectCount);
  TUint16 objectProgram[3 * objectCount],
    *ip = objectProgram;

  for (TInt row = 0; row < height; row++) {
    for (TInt col = 0; col < width; col++) {
      TUint32 tile = objectLayer.GetCell(row, col) & TUint32(0xffff);
      TUint16 attr = tlc[tile];
      if (tile != 0 && attr >= 16) {
//        printf("Found %d at row,col %d,%d\n", attr, row, col);
        *ip++ = attr;
        *ip++ = row;
        *ip++ = col;
      }
    }
  }
//  HexDump(objectProgram, 3*objectCount, 3*objectCount);

  sprintf(work, "%s_MAP", filename);
  resourceFile.StartResource(work);
  resourceFile.Write(&bmp_id, sizeof(bmp_id));
  resourceFile.Write(&objectCount, sizeof(objectCount));
  resourceFile.Write(&objectProgram[0], sizeof(objectProgram));

  resourceFile.Write(&map->width, sizeof(map->width));
  resourceFile.Write(&map->height, sizeof(map->height));
  resourceFile.Write(&data[0], width * height * sizeof(TUint32));

//  printf("objectProgram %d\n", sizeof(objectProgram));
}
