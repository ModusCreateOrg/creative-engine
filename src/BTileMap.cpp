//
// Created by Michael Schwartz on 2019-07-03.
//

#include "CreativeEngine.h"

#ifndef O_BINARY
#define O_BINARY (0)
#endif

struct TILE {
  TUint16 index;
  TUint8 flip, flop;
};
struct RAW {
  char stmp[4];
  TUint16 width, height;
  TILE data[];
};

BTileMap::BTileMap(const TUint8 *aData, const TUint16 *aTLC) {
  RAW *raw = (RAW *)&aData[0];
  this->mWidth = raw->width;
  this->mHeight = raw->height;
  this->mMapData = new TUint32[mWidth * mHeight];
  TUint32 *dst = &this->mMapData[0];
  TILE *src = raw->data;
  for (TUint h = 0; h < this->mHeight; h++) {
    for (TUint w = 0; w < this->mWidth; w++) {
      const TILE tile  = *src++;
      TUint16 index = TILE_INDEX(tile.index);
      index |= tile.flip ? (1 << 14) : 0;
      index |= tile.flop ? (1 << 15) : 0;
      const TUint16 attributes = aTLC[index];
      *dst++ = TUint32(attributes) << 16 | TUint32(index);
    }
  }
}

BTileMap::~BTileMap() {
  delete [] this->mMapData;
}

BTileMap *BTileMap::LoadFromFiles(const char *aPath, const char *aName) {
  int fd;
  off_t len;
  char buf[4096];

  sprintf(buf, "%s/%s.pmp#layer~Layer 1map001.stm", aPath, aName);
  fd = open(buf, O_RDONLY | O_BINARY);
  if (fd < 0) {
    Panic("BTileMap::LoadFromFiles: can't open %s\n", buf);
  }
  len = lseek(fd, 0, 2);
  lseek(fd, 0, 0);
  const TUint8 *data = new TUint8[len];
  read(fd, (void *)&data[0], size_t(len));
  close(fd);

  sprintf(buf, "%s/%s#tiles.tlc", aPath, aName);
  fd = open(buf, O_RDONLY | O_BINARY);
  if (fd < 0) {
    Panic("BTileMap::LoadFromFiles: can't open %s\n", buf);
  }
  len = lseek(fd, 0, 2);
  lseek(fd, 0, 0);
  const TUint16 *tlc = new TUint16[len/2];
  read(fd, (void *)&tlc[0], size_t(len));
  close(fd);

  auto *m = new BTileMap(data, tlc);
  return m;
}
