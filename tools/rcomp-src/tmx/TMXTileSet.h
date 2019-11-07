#ifndef RCOMP_TMXTILESET_H
#define RCOMP_TMXTILESET_H

class TMXTileSet {
public:
  TMXTileSet(const char *path, const char *filename, TUint32 firstgid);
  ~TMXTileSet();

public:
  void Dump();

public:
  TUint32 offsetGid;
  char file_name[MAX_STRING_LENGTH];  // file name without the path
  char tsx_name[MAX_STRING_LENGTH],
      tsx_path[MAX_STRING_LENGTH],
      bmp_name[MAX_STRING_LENGTH],
      bmp_path[MAX_STRING_LENGTH];

  TInt16 num_tiles;
  TUint32 *attributes;
};

#endif //RCOMP_TMXTILESET_H
