#include "rcomp.h"
#include "TMXFile.h"
#include "lib/utils.h"
#include "tmx/TMXMap.h"

TMXFile::TMXFile(const char *path, const char *filename) {
  char fn[MAX_STRING_LENGTH],
    line[MAX_STRING_LENGTH],
    token[MAX_STRING_LENGTH],
    attr[MAX_STRING_LENGTH], value[MAX_STRING_LENGTH];

  sprintf(fn, "%s/%s", path, filename);
  printf("TMXFile '%s' '%s' = '%s'\n", path, filename, fn);
  RawFile txt(fn);
  if (!txt.alive) {
    Panic("Could not open %s\n", fn);
  }

  while (txt.ReadLine(line)) {
    TMXMap map(path, line);
  }

}

TMXFile::~TMXFile() {
  //
}
