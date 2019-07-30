//
// Created by mschwartz on 7/25/19.
//

#include "ResourceFile.h"

ResourceFile resourceFile;

ResourceFile::ResourceFile() {
  defines = fopen("Resources.h", "w");
  if (!defines) {
    printf("Can't open output file resourceFile.h (%d)\n", errno);
    exit(1);
  }

  bin = fopen("Resources.bin", "w");
  if (!bin) {
    printf("Can't open binary file resourceFile.bin (%d)\n", errno);
    exit(1);
  }

  strcpy(path, "");

  resource_number       = 0; // resource_number into offset table at beginning of compiled data
  offset      = 0;
  output     = (uint8_t *) malloc(4096);
  output_size = 4096;
}

TUint16 ResourceFile::StartResource(char *define_name) {
  TUint16 ret = resource_number;
  generate_define_name(define_name);
  fprintf(defines, "#define %-64.64s %d\n", define_name, resource_number);
  offsets[resource_number++] = offset;
  return ret;
}

void ResourceFile::Write(void *data, long size) {
  uint32_t next_offset = offset + size;
  if (next_offset > output_size) {
    output_size = next_offset;
    output      = (uint8_t *) realloc(output, output_size);
  }
  memcpy(&output[offset], data, size);
  offset += size;
}

void ResourceFile::Finish() {
  fwrite(&resource_number, sizeof(resource_number), 1, bin);
  fwrite(offsets, resource_number, sizeof(offsets[0]), bin);
  fwrite(output, offset, 1, bin);
  fprintf(defines, "\n#define %-64.64s %d\n", "NUM_RESOURCES", resource_number);
  fclose(defines);
  fclose(bin);
}

