#include "rcomp.h"
#include "TMX.h"

struct TSX {
  char name[2048];
  BMPFile *bmp;
  TInt num_tiles;
  TUint32 *tiles;
};

struct Level {
  Level() {
    mapLayer = objectsLayer = objectAttributesLayer = ENull;
  }

  TInt16 width, height;
  TUint16 tileWidth, tileHeight;
  TUint32 *mapLayer, *objectsLayer, *objectAttributesLayer;
};

void parse_attr(char *s, char *attr, char *value) {
  char *x = attr;
  while (*s && *s != '=') {
    *attr++ = *s++;
  }
  *attr = '\0';
  if (*s) {
    s++;
  }
  if (*s) {
    s++;
  }
  while (*s && *s != '"') {
    *value++ = *s++;
  }
  *value = '\0';
}

TBool parse_value(char *s, const char *attribute, char *value) {
  char token[2048], attr[2048];
  while (*s && (s = parse_token(token, s))) {
    parse_attr(token, attr, value);
    if (strcasecmp(attr, attribute) == 0) {
      return ETrue;
    }
  }
  return EFalse;
}

TSX *parse_tsx(const char *path, char *filename) {
  char line[2048],
    token[2048],
    attr[2048], value[2048];

  TSX *tsx = new TSX();
  tsx->bmp = ENull;

  sprintf(line, "%s/%s", path, filename);
  RawFile f(line);
  if (!f.alive) {
    Panic("*** parse_tsx: Can't open tsx %s.\n", filename);
  }

  f.ReadLine(line); // skip xml line

  f.ReadLine(line);
  char *ptr = line;
  ptr = parse_token(token, ptr);
  if (strcasecmp(token, "<tileset") != 0) {
    Panic("*** parse_tsx (%s) expected <tileset> tag (%s)\n", filename, line);
  }

  if (!parse_value(ptr, "name", value)) {
    Panic("*** parse_tsx (%s) expected name attribute(%s)\n", filename, line);
  }
  strcpy(tsx->name, value);

  if (!parse_value(ptr, "tilecount", value)) {
    Panic("*** parse_tsx (%s) expected tilecount attribute(%s)\n", filename, line);
  }
  tsx->num_tiles = atoi(value);
  tsx->tiles = new TUint32[tsx->num_tiles];

  // parse image
  f.ReadLine(line);
  ptr = parse_token(token, line);
  if (strcasecmp(token, "<image") != 0) {
    Panic("*** parse_tsx (%s) expected image tag(%s)\n", filename, line);
  }
  if (!parse_value(ptr, "source", value)) {
    Panic("*** parse_tsx (%s) expected source attribute (%s)\n", filename, line);
  }
  sprintf(attr, "%s/%s/%s", path, dirname(filename), value);
  printf("attr (%s), value (%s), dirname(%s)\n", attr, value, dirname(filename));
  realpath(attr, value);
  printf("attr (%s), value (%s), dirname(%s)\n", attr, value, dirname(filename));
  tsx->bmp = new BMPFile(attr);

  while ((ptr = parse_token(token, ptr)) && *ptr) {
    parse_attr(token, attr, value);
    printf("attr (%s) value(%s)\n", attr, value);
  }
  exit(1);

}

TMX::TMX(const char *path, char *filename) {
  char fn[2048],
    line[2048],
    token[2048],
    attr[2048], value[2048];

  sprintf(fn, "%s/%s", path, filename);
  printf("TMX '%s' '%s' = '%s'\n", path, filename, fn);
  RawFile txt(fn);
  if (!txt.alive) {
    Panic("Could not open %s\n", fn);
  }

  while (txt.ReadLine(line)) {
    sprintf(token, "%s/%s", path, line);
    realpath(token, fn);
    printf("  %s\n", fn);
    RawFile f(fn);
    if (!f.alive) {
      Panic("Could not open %s\n", fn);
    }
    f.ReadLine(line);  // xml line, skip it
    f.ReadLine(line);
    printf("line '%s\n", line);

    TUint16 width = 0, height = 0, tileWidth = 0, tileHeight = 0;

    char *ptr = line;
//    char *ptr = parse_token(token, line);
    while ((ptr = parse_token(token, ptr)) && *ptr) {
      printf("token %s\n", token);
      parse_attr(token, attr, value);
      printf("  attr(%s) value(%s)\n", attr, value);
      if (!strcasecmp(attr, "width")) {
        width = atoi(value);
      }
      else if (!strcasecmp(attr, "height")) {
        height = atoi(value);
      }
      else if (!strcasecmp(attr, "tilewidth")) {
        tileWidth = atoi(value);
      }
      else if (!strcasecmp(attr, "tileheight")) {
        tileHeight = atoi(value);
      }
    }

    printf("width,height %d,%d tilesize %dx%d\n", width, height, tileWidth, tileHeight);

    f.ReadLine(line);
    ptr = parse_token(token, line);
    parse_attr(token, attr, value);
    if (strcasecmp(attr, "<tileset") == 0) {
      if (!parse_value(ptr, "source", value)) {
        Panic("*** TMX: tsx (%s) not found\n", line);
      }
      parse_tsx(path, value);
    }

    while ((ptr = parse_token(token, ptr)) && *ptr) {
      printf("token %s\n", token);
      parse_attr(token, attr, value);
    }

    exit(1);

  }

}

TMX::~TMX() {
  //
}
