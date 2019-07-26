//
// Created by mschwartz on 7/25/19.
//

#include "RawFile.h"

RawFile::RawFile(const char *aFilename) {
  filename = strdup(aFilename);
  alive    = EFalse;
  int fd   = open(filename, O_RDONLY | O_BINARY);
  if (fd < 1) {
    return;
  }
  this->size = (TUint32) lseek(fd, 0, 2);
  lseek(fd, 0, 0);
  this->data = new TUint8[size];
  read(fd, this->data, size);
  close(fd);
  alive = ETrue;
}

TUint32 RawFile::OutputSize() {
  return sizeof(size) + size;
}

