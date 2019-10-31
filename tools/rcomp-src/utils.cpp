#include "rcomp.h"

void HexDump(TUint8 *ptr, TInt length, TInt width) {
  TUint32 addr = 0;
  TInt count = 0;
  while (length > 0) {
    printf("%08x ", addr);
    for (TInt i = 0; i < width && --length > 0; i++) {
      printf("%02x ", *ptr++);
      count++;
      if (count > width - 1) {
        count = 0;
        addr += width;
        break;
      }
    }
    printf("\n");
  }
}

void HexDump(TInt16 *ptr, TInt length, TInt width) {
  TUint32 addr = 0;
  TInt count = 0;
  while (length > 0) {
    printf("%08x ", addr);
    for (TInt i = 0; i < width && --length > 0; i++) {
      printf("%04x ", *ptr++);
      count++;
      if (count > width - 1) {
        count = 0;
        addr += width;
        break;
      }
    }
    printf("\n");
  }
}

void HexDump(TUint32 *ptr, TInt length, TInt width) {
  TUint32 addr = 0;
  TInt count = 0;
  while (length > 0) {
    printf("%08x ", addr);
    for (TInt i = 0; i < width && --length > 0; i++) {
      printf("%08x ", *ptr++);
      count++;
      if (count > width - 1) {
        count = 0;
        addr += width;
        break;
      }
    }
    printf("\n");
  }
}

// return pointer to bit of string after blanks.
char *skipbl(char *p) {
  while (*p && isspace(*p)) {
    p++;
  }
  return p;
}

// put nex ttoken from the line at src into the char bufer at dst, return pointer to the character after the token.
char *parse_token(char *dst, char *src) {
  src = skipbl(src);
  while (*src && !isspace(*src)) {
    *dst++ = *src++;
  }
  *dst = '\0';
  return src;
}

// trim leading and trailing whitespace
char *trim(char *p) {
  while (isspace(*p))
    p++;
  for (char *end = &p[strlen(p) - 1]; end > p && isspace(*end);) {
    *end-- = '\0';
  }

  return p;
}

/**
 * Convert a filename to an all uppercase #define name.
 *
 * @param filename
 * @return
 */
void generate_define_name(char *base) {
  for (TInt i = 0; base[i]; i++) {
    switch (base[i]) {
      case '.':
      case '#':
      case '~':
      case ' ':
      case '-':
        base[i] = '_';
        break;
      default:
        base[i] = (char)toupper(base[i]);
    }
  }
}
