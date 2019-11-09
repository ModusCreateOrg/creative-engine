#include "rcomp.h"
#include "TMXLayer.h"

TMXLayer::TMXLayer(RawFile &aFile, char line[MAX_STRING_LENGTH]) {
  char value[MAX_STRING_LENGTH];
  data = ENull;

  parse_value(line, "id", value);
  id = atol(value);

  parse_value(line, "name", name);

  parse_value(line, "width", value);
  width = atol(value);

  parse_value(line, "height", value);
  height = atol(value);

  printf("  layer: %s is %dx%d\n", name, width, height);
  data = new TUint32[width * height];

  // <data> tag
  if (!aFile.ReadLine(line)) {
    Panic("Unexpected EOF\n");
  }

  TUint32 *ptr = data;
  for (TInt h = 0; h < height; h++) {
    if (!aFile.ReadLine(line)) {
      Panic("Unexpected EOF\n");
    }
    char *src = line;
    while (*src) {
      TUint32 n = 0;
      while (*src && *src != ',') {
        n *= 10;
        n += *src++ - '0';
      }
      *ptr++ = n;
      if (*src == ',') {
        src++;
      }
    }
  }

  // </data> tag
  if (!aFile.ReadLine(line)) {
    Panic("Unexpected EOF\n");
  }
  // </layer> tag
  if (!aFile.ReadLine(line)) {
    Panic("Unexpected EOF\n");
  }

}

TMXLayer::~TMXLayer() {
  delete[] data;
}

void TMXLayer::Dump() {
  printf("\n TMXLayer(%s) is %dx%d\n", name, width, height);
  TUint32 *ptr = data;
  for (TInt h=0; h<height; h++) {
    for (TInt w=0; w<width; w++) {
      printf("%5d ", *ptr++);
    }
    printf("\n");
  }
  printf("\n");
}
