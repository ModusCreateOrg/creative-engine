#ifndef MODITE_BMPFILE_H
#define MODITE_BMPFILE_H

#include "lib/RawBitmap.h"
#include "rcomp.h"

class BMPHEADER;
class ResourceFile;

class BMPFile : public RawBitmap {
private:
  TUint8 *rawFile;
  BMPHEADER *bmp;

public:
  BMPFile(const char *filename);
  ~BMPFile();

public:
  void Write(ResourceFile &resourceFile);
  TUint8 ReadPixel(TInt x, TInt y);
};

#endif //MODITE_BMPFILE_H
