#include "RawFile.h"
#include <string.h>

RawFile::RawFile(const char *aFilename) {
  filename = strdup(aFilename);
  alive = EFalse;
  int fd = open(filename, O_RDONLY | O_BINARY);
  if (fd < 1) {
    return;
  }
  this->size = (TUint32) lseek(fd, 0, 2);
  lseek(fd, 0, 0);
  this->data = new TUint8[size];
  read(fd, this->data, size);
  close(fd);
  alive = ETrue;
  Rewind();
}

RawFile::~RawFile() {
  this->input = ENull;
  delete[] this->data;
  alive = EFalse;
}

TUint32 RawFile::OutputSize() {
  return sizeof(size) + size;
}

char *RawFile::ReadLine(char *line) {
  if (!alive || input == ENull) {
    return ENull;
  }

  auto *dst = (TUint8 *) line;
  while (input - data < size) {
    if (*input == '\r') {
      input++;
      continue;
    }
    if (*input == '\n') {
      input++;
      *dst = '\0';
      break;
    }
    *dst++ = *input++;
  }
  *dst = '\0';
  if (input - data >= size) {
    input = ENull;
  }
//  printf("`%s`\n", line);
  return line;
}

