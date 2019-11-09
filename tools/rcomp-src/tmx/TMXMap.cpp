#include "rcomp.h"
#include "TMXMap.h"
#include "TMXTileSet.h"
#include "TMXLayer.h"
#include "BTileMap.h"

#define DEBUGME
//#undef DEBUGNE

TMXMap::TMXMap(const char *path, const char *filename) {
  char fn[MAX_STRING_LENGTH],
    line[MAX_STRING_LENGTH],
    token[MAX_STRING_LENGTH],
    value[MAX_STRING_LENGTH];

  this->path_buf = strdup(path);
  this->path = strrchr(this->path_buf, '/');
  if (this->path) {
    this->path++;   // skip over the /
  }
  else {
    this->path = this->path_buf;
  }

  this->name_buf = strdup(filename);
  this->name = strrchr(this->name_buf, '/');
  if (this->name) {
    this->name++;   // skip over the /
  }
  else {
    this->name = this->name_buf;
  }
  char *tmx_ptr = this->name;
  while (*tmx_ptr) {
    if (strncasecmp(tmx_ptr, ".TMX", 4) == 0) {
      *tmx_ptr++ = '_';
      *tmx_ptr++ = 'M';
      *tmx_ptr++ = 'A';
      *tmx_ptr = 'P';
      break;
    }
    tmx_ptr++;
  }

  map_attributes = objects = objects_attributes = ENull;
  sprintf(token, "%s/%s", path, filename);
  realpath(token, fn);
  RawFile f(fn);
  if (!f.alive) {
    Panic("Could not open %s\n", fn);
  }
  f.ReadLine(line); // xml line, skip it
  f.ReadLine(line);

  parse_value(line, "width", value);
  width = atoi(value);
  parse_value(line, "height", value);
  height = atoi(value);
  parse_value(line, "tilewidth", value);
  tileWidth = atoi(value);
  parse_value(line, "tileheight", value);
  tileHeight = atoi(value);

  printf("%s is width,height %d,%d tilesize %dx%d\n", filename, width, height, tileWidth, tileHeight);
  data = new TUint32[width * height];
  // NOTE: we loop and clear so we're truly portable.  Operator new may or may not zero allocated memory as
  // far as Creative Engine is concerned.
  for (auto i = 0; i < width * height; i++) {
    data[i] = 0;
  }
  for (unsigned int &attribute : attributes) {
    attribute = 0;
  }

  while (ETrue) {
    f.ReadLine(line);
    parse_token(token, line);
    if (strcasecmp(token, "<tileset") != 0) {
      break;
    }

    if (!parse_value(line, "firstgid", value)) {
      Panic("*** TMXFile: firstgid (%s) not found\n", line);
    }
    TUint32 firstgid = atol(value);

    if (!parse_value(line, "source", value)) {
      Panic("*** TMXFile: source (%s) not found\n", line);
    }
    printf(">>> tileset %s (%d)\n", value, firstgid);
    auto *tileset = new TMXTileSet(path, value, firstgid);
//    tileset->Dump();

    for (TInt i = 0; i < tileset->num_tiles; i++) {
//            printf("%d(%d) -> %x/%d\n", firstgid, firstgid + i, tileset->attributes[i], tileset->attributes[i]);
      attributes[firstgid + i] = tileset->attributes[i];
    }

    if (strcasecmp(tileset->tsx_name, "MAP_LAYER") == 0) {
      map_attributes = tileset;
    } else if (strcasecmp(tileset->tsx_name, "OBJECT_LAYER") == 0) {
      objects = tileset;
    } else if (strcasecmp(tileset->tsx_name, "OBJECT_ATTRIBUTES_LAYER") == 0) {
      objects_attributes = tileset;
    } else {
      Panic("Invalid tsx_name %s\n", tileset->tsx_name);
    }
  }

  if (map_attributes == ENull) {
    Panic("MAP_LAYER tileset not found for %s\n", filename);
  }
  if (objects == ENull) {
    Panic("OBJETS tileset not found for %s\n", filename);
  }
  if (objects_attributes == ENull) {
    Panic("OBJECT_ATTRIBUTE_LAYER tileset not found for %s\n", filename);
  }

  while (ETrue) {
    parse_token(token, line);
    if (strcasecmp(token, "<layer") != 0) {
      break;
    }
    auto *l = new TMXLayer(f, line);
    if (strcasecmp(l->name, "MAP_LAYER") == 0) {
      map_layer = l;
    } else if (strcasecmp(l->name, "MAP_ATTRIBUTES_LAYER") == 0) {
      map_attributes_layer = l;
    } else if (strcasecmp(l->name, "OBJECT_LAYER") == 0) {
      objects_layer = l;
    } else if (strcasecmp(l->name, "OBJECT_ATTRIBUTES_LAYER") == 0) {
      objects_attributes_layer = l;
    } else {
      Panic("*** Invalid Layer in TMXMap '%s': (%s)\n", l->name);
    }
    if (!f.ReadLine(line)) {
      break;
    }
  }

  if (map_layer == ENull) {
    Panic("map_layer not found for %s\n", filename);
  }
  if (map_attributes_layer == ENull) {
    Panic("map_attributes_layer not found for %s\n", filename);
  }
  if (objects_layer == ENull) {
    Panic("objects_layer not found for %s\n", filename);
  }
  if (objects_attributes_layer == ENull) {
    Panic("objects_attributes_layer not found for %s\n", filename);
  }

  //
  // set the actual map values
  //

  // tile indexes for rendering in low word
  for (TInt n = 0; n < width * height; n++) {
    const TUint32 tile = map_layer->data[n] - 1;
    const TUint32 atile = map_attributes_layer->data[n] -1;
    const TUint32 attr = attributes[atile];
    data[n] = (attr<<16) | tile;
  }

#if 0
  if (strcasecmp(l->name, "MAP_LAYER") == 0) {
    for (TInt i=0; i<width*height; i++) {
      data[i] |= l->data[i];
    }
  }
  else {
    for (TInt i=0; i<width*height; i++) {
      data[i] |= attributes[l->data[i]];
    }
  }
#endif

//  map_layer->Dump();
//  map_attributes->Dump();
//  objects_layer->Dump();
//  objects_attributes->Dump();
  ProcessObjects();
  Dump();
}

void TMXMap::ProcessObjects() {
  TUint32 gid = objects->offsetGid;
  TUint32 *object_layer_data = &objects_layer->data[0];
  TUint32 *object_data_attributes = &objects_attributes_layer->data[0];

  objectCount = 0;
  for (TInt row = 0; row < height; row++) {
    for (TInt col = 0; col < width; col++) {
      const TInt index = row * width + col;
      const TUint32 tile = LOWORD(object_layer_data[index]);
      if (!tile) {
        continue;
      }
//      const TUint32 attr = tile - gid; // objects->attributes[tile - gid];
      const TUint32 attr = tile; // objects->attributes[tile - gid];
      printf("FOUND %d($%x/%d) at %d,%d\n", tile, attributes[attr], attributes[attr], row, col);
      objectCount++;
    }
  }

#ifdef DEBUGME
  printf("  (Found %d objects)\n", objectCount);
#endif

  auto *objectProgram = new BObjectProgram[objectCount],
    *ip = &objectProgram[0];

  if (objectCount) {
    for (TInt row = 0; row < height; row++) {
      for (TInt col = 0; col < width; col++) {
        const TInt index = row * width + col;
        const TUint32 tile = LOWORD(object_layer_data[index]);
        const TUint32 attr1 = attributes[tile];
        const TUint32 tile2 = LOWORD(object_data_attributes[index]);
        const TUint32 attr2 = attributes[tile2];
        const TUint32 attr = attr2 << TUint32(16) | attr1;

        if (tile && attr) {
#ifdef DEBUGME
          printf("found tile:%d tile2:%d at row,col:%d,%d attr1:%d attr2:%d attr:%08x\n", tile, tile2, row, col,
                 attr1, attr2, attr);
#endif
          ip->mCode = attr;
          ip->mRow = row;
          ip->mCol = col;
          ip++;
        }
      }
    }
  }
  mObjectProgram = objectProgram;
}

TMXMap::~TMXMap() {
  delete[] name_buf;
  delete map_attributes;
  delete objects;
  delete objects_attributes;
}

void TMXMap::Dump() {
  char symbol[MAX_STRING_LENGTH];
  strcpy(symbol, map_attributes->bmp_name);
  symbol_name(symbol);
  BSymbol *sym = resourceFile.symbols.LookupSymbol(symbol);
  printf("\n TMXMap(%s) is %dx%d %s (%s)\n", name, width, height, symbol, sym ? "duplicate" : "new");
  TUint32 *ptr = data;
  for (TInt h=0; h<height; h++) {
    for (TInt w=0; w<width; w++) {
      printf("%08x ", *ptr++);
    }
    printf("\n");
  }
  printf("\n");
}

void TMXMap::Write(ResourceFile &resourceFile) {
  char symbol[MAX_STRING_LENGTH];
  strcpy(symbol, map_attributes->bmp_name);
  symbol_name(symbol);
  BSymbol *sym = resourceFile.symbols.LookupSymbol(symbol);
  //
  TUint16 bmp_id;
  if (sym) {
    bmp_id = sym->value;
  }
  else {
    bmp_id = resourceFile.StartResource(symbol);
    BMPFile bmp(map_attributes->bmp_path);
    bmp.Write(resourceFile);
  }

  sprintf(symbol, "%s_%s", path, name);
  symbol_name(symbol);
  if (resourceFile.symbols.LookupSymbol(symbol)) {
    Panic("Duplicate MAP %s\n", symbol);
  }

  resourceFile.StartResource(symbol);
  resourceFile.Write(&bmp_id, sizeof(bmp_id));
  resourceFile.Write(&objectCount, sizeof(objectCount));
  resourceFile.Write(&mObjectProgram[0], sizeof(BObjectProgram) * objectCount);

  resourceFile.Write(&width, sizeof(width));
  resourceFile.Write(&height, sizeof(height));
  resourceFile.Write(&data[0], width * height * sizeof(TUint32));
}
