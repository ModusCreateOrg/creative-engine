#include "RawFile.h"
#include "BTileMap.h"
#include "TileMap.h"
#include "stdio.h"

#define DEBUGME
//#undef DEBUGME

TInt ParseLayer(const char *str) {
  const char *pos = strcasestr(str, "Level_");
  if (pos == ENull) {
    printf("Level_ is required in filename (%s)\n", str);
    exit(1);
  }
  pos = &pos[6];
  TInt level = 0;
  while (isdigit(*pos)) {
    level *= 10;
    level += *pos - '0';
    pos++;
  }
  return level;
}

TileMap::TileMap(const char *path, const char *filename) {
  this->filename      = strdup(filename);
  this->mapAttributes = ENull;
  this->bmp           = ENull;
  for (TInt i = 0; i < MAX_LEVELS; i++) {
    mLevels[i].used              = EFalse;
    mLevels[i].map               = ENull;
    mLevels[i].map_attributes    = ENull;
    mLevels[i].object            = ENull;
    mLevels[i].object_attributes = ENull;
  }

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
    } else if (!strcasecmp(extension, "tlc")) {
      printf("-> %s\n", ptr);
      this->mapAttributes = new RawFile(resourceFn);
    } else if (!strcasecmp(extension, "stm")) {
      if (strcasestr(ptr, "MAP_LAYER") != ENull) {
        printf("-> MAP LAYER %s\n", ptr);
        TInt level = ParseLayer(ptr);
        mLevels[level].used = ETrue;
        mLevels[level].map  = new RawFile(resourceFn);
      } else if (strcasestr(ptr, "MAP_ATTRIBUTE_LAYER") != ENull) {
        printf("-> MAP ATTRIBUTES %s\n", ptr);
        TInt level = ParseLayer(ptr);
        mLevels[level].used           = ETrue;
        mLevels[level].map_attributes = new RawFile(resourceFn);
      } else if (strcasestr(ptr, "OBJECT_LAYER")) {
        printf("-> OBJECT LAYER %s\n", ptr);
        TInt level = ParseLayer(ptr);
        mLevels[level].used   = ETrue;
        mLevels[level].object = new RawFile(resourceFn);
      } else if (strcasestr(ptr, "OBJECT_ATTRIBUTE_LAYER") != ENull) {
        printf("-> OBJECT ATTRIBUTES %s\n", ptr);
        TInt level = ParseLayer(ptr);
        mLevels[level].used              = ETrue;
        mLevels[level].object_attributes = new RawFile(resourceFn);
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

void Dump() {

}

struct MapData {
  char    token[4];
  TUint16 width, height;
  TUint32 data[];
};

#if 0
struct ObjectLayer {
  ObjectLayer(TInt32 aWidth, TInt32 aHeight, TUint32 *aData, TUint32 size) {
    width  = aWidth;
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

  TInt32  width, height;
  TUint32 *data;
};
#endif

static void make_filename(char *dst, const char *src) {
  while (*src) {
    if (!strncasecmp(src, "FILELIST.TXT", 13)) {
      src += 13;
    } else {
      *dst++ = *src++;
    }
  }
  *dst = '\0';
}

void TileMap::Write(ResourceFile &resourceFile) {
  if (!mapAttributes) {
    abort("TileMap (%s) missing TLC\n:", filename);
  }
  if (!bmp) {
    abort("TileMap (%s) missing BMP\n:", filename);
  }

  // write out the BMP
  char work[2048];
  make_filename(work, filename);
  strcat(work, "BMP");
  TUint16 bmp_id = resourceFile.StartResource(work);
  bmp->Write(resourceFile);

  TUint16   *tlc = (TUint16 *) mapAttributes->data;
  for (TInt i    = 0; i < MAX_LEVELS; i++) {
    LayerInfo *layer = &mLevels[i];
    if (!layer->used) {
      continue;
    }
    if (!layer->map || !layer->map_attributes || !layer->object || !layer->object_attributes) {
      abort("Level %d is incomplete\n", i);
    }
    MapData *map               = (MapData *) layer->map->data;
    MapData *map_attributes    = (MapData *) layer->map_attributes->data;
    MapData *object            = (MapData *) layer->object->data;
    MapData *object_attributes = (MapData *) layer->object_attributes->data;
    TInt32  width              = map->width,
            height             = map->height;

    TUint32 *map_data               = &map->data[0],
            *map_data_attributes    = &map_attributes->data[0];
    TUint32 *object_data            = &object->data[0],
            *object_data_attributes = &object_attributes->data[0];

    printf("--> TILEMAP %s Level %d is %dx%d\n", filename, i, map->width, map->height);
    // We set the attributes bits in the map so the game can fiddle the individual tiles' bits during play.
    for (TInt n = 0; n < map->width * map->height; n++) {
      map_data[n] |= TUint32(tlc[map_data[n]] << TUint32(16));
      map_data[n] |= TUint32(tlc[map_data_attributes[n]]) << TUint16(16);
    }
    HexDump(map_data, width, width);

    TUint16   objectCount = 0;
    for (TInt row         = 0; row < height; row++) {
      for (TInt col = 0; col < width; col++) {
        const TInt    index = row * width + col;
        const TUint32 tile  = LOWORD(object_data[index]);
        const TUint32 attr1 = tlc[tile];
        const TUint32 tile2 = LOWORD(object_data_attributes[index]);
        const TUint32 attr2 = tlc[tile2];
        const TUint32 attr  = attr2 << TUint32(16) | attr1;
        if (tile && attr) {
          objectCount++;
        }
      }
    }

    printf("Found %d objects\n", objectCount);

    BObjectProgram objectProgram[objectCount],
                   *ip    = &objectProgram[0];

    if (objectCount) {
      for (TInt row = 0; row < height; row++) {
        for (TInt col = 0; col < width; col++) {
          const TInt    index = row * width + col;
          const TUint32 tile  = LOWORD(object_data[index]);
          const TUint32 attr1 = tlc[tile];
          const TUint32 tile2 = LOWORD(object_data_attributes[index]);
          const TUint32 attr2 = tlc[tile2];
          const TUint32 attr  = attr2 << TUint32(16) | attr1;

          if (tile && attr) {
#ifdef DEBUGME
            printf("found tile:%d tile2:%d at row,col:%d,%d attr1:%d attr2:%d attr:%08x\n", tile, tile2, row, col,
                   attr1, attr2, attr);
#endif
            ip->mCode = attr;
            ip->mRow  = row;
            ip->mCol  = col;
            ip++;
          }
        }
      }
    }

//    ObjectLayer objectLayer(width, height, (TUint32 *) &object->data[4 * sizeof(TUint16)],
//                            width * height); // skip over STM and width,height

    make_filename(work, filename);
    sprintf(&work[strlen(work)], "LEVEL%d_MAP", i);

    resourceFile.StartResource(work);
    resourceFile.Write(&bmp_id, sizeof(bmp_id));
    resourceFile.Write(&objectCount, sizeof(objectCount));
    resourceFile.Write(&objectProgram[0], sizeof(BObjectProgram) * objectCount);

    resourceFile.Write(&map->width, sizeof(map->width));
    resourceFile.Write(&map->height, sizeof(map->height));
    resourceFile.Write(&map_data[0], width * height * sizeof(TUint32));
  }
//  printf("objectProgram %d\n", sizeof(objectProgram));
}
