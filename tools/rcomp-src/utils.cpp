//
// Created by mschwartz on 7/25/19.
//
#include "rcomp.h"

void abort(const char *message, ...) {
  va_list args;
  va_start(args, message);
  vfprintf(stderr, message, args);
  va_end(args);
  exit(EXIT_FAILURE);
}

void HexDump(TUint8 *ptr, int length) {
  TUint32  addr = 0;
  TInt count = 0;
  while (length > 0) {
    printf("%08x ", addr);
    for (int i = 0; i < 8 && --length > 0; i++) {
      printf("%02x ", *ptr++);
      count++;
      if (count > 7) {
        count = 0;
        addr += 8;
        break;
      }
    }
    printf("\n");
  }
}

// return pointer to bit of string after blanks.
char *skipbl(char *p) {
  while (isspace(*p)) {
    p++;
  }
  return p;
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
  for (int i = 0; base[i]; i++) {
    switch (base[i]) {
      case '.':
      case '#':
      case '~':
      case ' ':
      case '-':
        base[i] = '_';
        break;
      default:
        base[i]   = (char) toupper(base[i]);
    }
  }
}
