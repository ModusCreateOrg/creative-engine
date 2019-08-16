//
// Created by mschwartz on 7/25/19.
//

#ifndef MODITE_RESOURCEFILE_H
#define MODITE_RESOURCEFILE_H

#include "rcomp.h"

// maximum number of resources allowed in a .bin file
const TInt MAX_RESOURCE_FILES=1024;

class ResourceFile {
public:
  ResourceFile();
public:
  TUint16 StartResource(char *define_name);
  void Write(void *data, long size);
  void Finish();
public:
  char path[2048];

  FILE *defines;
  FILE *bin;

  uint32_t resource_number; // resource_number into offset table at beginning of compiled data
  uint32_t offset;
  uint32_t offsets[MAX_RESOURCE_FILES];
  uint8_t  *output;
  uint32_t output_size;
};

extern ResourceFile resourceFile;

#endif //MODITE_RESOURCEFILE_H
