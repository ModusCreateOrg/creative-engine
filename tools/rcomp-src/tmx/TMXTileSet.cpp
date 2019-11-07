#include "../rcomp.h"
#include "TMXTileSet.h"

TMXTileSet::TMXTileSet(const char *path, const char *filename, TUint32 firstgid) {
  offsetGid = firstgid;

  char work[MAX_STRING_LENGTH],
      line[MAX_STRING_LENGTH],
      token[MAX_STRING_LENGTH],
      value[MAX_STRING_LENGTH];

  strcpy(file_name, filename);
  sprintf(work, "%s/%s", path, filename);
  realpath(work, tsx_path);

  RawFile f(tsx_path);
  if (!f.alive) {
    Panic("*** TMXTileSet: Can't open %s.\n", filename);
  }

  f.ReadLine(line); // skip xml line

  // parse tileset tag
  f.ReadLine(line);
  char *ptr = line;
  ptr = parse_token(token, ptr);
  if (strcasecmp(token, "<tileset") != 0) {
    Panic("*** parse_tsx (%s) expected <tileset> tag (%s)\n", filename, line);
  }

  if (!parse_value(ptr, "name", value)) {
    Panic("*** parse_tsx (%s) expected name attribute(%s)\n", filename, line);
  }
  strcpy(tsx_name, value);

  if (!parse_value(ptr, "tilecount", value)) {
    Panic("*** parse_tsx (%s) expected tilecount attribute(%s)\n", filename, line);
  }
  num_tiles = atoi(value);
  attributes = new TUint32[num_tiles];
  for (TInt n=0; n<num_tiles; n++) {
    attributes[n] = 0;
  }

  // parse image tag
  f.ReadLine(line);
  ptr = parse_token(token, line);
  if (strcasecmp(token, "<image") != 0) {
    Panic("*** parse_tsx (%s) expected image tag(%s)\n", filename, line);
  }
  if (!parse_value(ptr, "source", bmp_name)) {
    Panic("*** parse_tsx (%s) expected source attribute (%s)\n", filename, line);
  }
  sprintf(work, "%s/%s/%s", path, dirname(file_name), bmp_name);

  realpath(work, bmp_path);

  // process the <tile> tags
  TInt index = 0;
  while (f.ReadLine(line)) {
    parse_token(token, line);
    if (strcasecmp(token, "<tile") != 0) {
      break;
    }
    f.ReadLine(line); // <properties>
    f.ReadLine(line); // <property>
    parse_value(line, "value", value);
    TUint32 v = atol(value);
    //    printf("index/value = %d/%d\n", index, v);
    attributes[index++] = v;
    f.ReadLine(line); // </properties>
    f.ReadLine(line); // </tile>
  }
}

TMXTileSet::~TMXTileSet() {
  delete[] attributes;
}

void TMXTileSet::Dump() {
  printf("%16.16s: %s\n", "tsx_name", tsx_name);
  printf("%16.16s: %s\n", "tsx_path", tsx_path);
  printf("%16.16s: %s\n", "bmp_name", bmp_name);
  printf("%16.16s: %s\n", "bmp_path", bmp_path);
  printf("%16.16s: %d\n", "gid", offsetGid);
  printf("%16.16s: %d\n", "num_tiles", num_tiles);
  for (TInt i = 0; i < num_tiles; i++) {
//    printf("%d: %x\n",i, attributes[i] );
    if ((i % 16) == 0) {
      if (i) {
        printf("\n%16.16s  ");
      }
      else {
        printf("%16.16s: ", "attributes");
      }
    }
    printf("%04x ", attributes[i]);
  }
  printf("\n");
}
