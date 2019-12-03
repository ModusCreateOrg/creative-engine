#include "../rcomp.h"

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
void symbol_name(char *dst, char *base) {
  if (isdigit(*base)) {
    *dst++ = 'P';
  }
  for (TInt i = 0; base[i]; i++) {
    switch (base[i]) {
      case '.':
      case '#':
      case '~':
      case ' ':
      case '-':
        *dst++ = '_';
        break;
      default:
        *dst++ = (char)toupper(base[i]);
        break;
    }
  }
  *dst = '\0';
}

// parse xml attribute, e.g. foo="bar", into attr[] = foo, value = bar
void parse_attr(char *s, char *attr, char *value) {
  s = skipbl(s);
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

// parse xml value for given attribute - e.g. given attribute "foo" and xml contains foo="bar", then value is set to "bar"
TBool parse_value(char *s, const char *attribute, char *value) {
  char token[MAX_STRING_LENGTH], attr[MAX_STRING_LENGTH];
  while (*s && (s = parse_token(token, s))) {
    parse_attr(token, attr, value);
    if (strcasecmp(attr, attribute) == 0) {
      return ETrue;
    }
  }
  return EFalse;
}

