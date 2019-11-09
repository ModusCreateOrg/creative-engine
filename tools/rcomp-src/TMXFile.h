#ifndef RCOMP_TMXFILE_H
#define RCOMP_TMXFILE_H

class BMPFile;

class TMXMap;
//#include "tmx/TMXTileSet.h"
//#include "tmx/TMXMap.h"

/**
  * This represents the .txt file for TileEd created mapped playfields.
  *
  * The .txt file lists a number of .tmx files.  The .tmx files each contain
  * a number of tileset definitions, and 4 layers.  A layer is a 2 dimsensional map of 
  * indexes into the tileset.
  * 
  */
class TMXFile {
public:
  TMXFile(const char *path, const char *filename);
  ~TMXFile();
public:
  const char *filename;
  TMXMap *mLevels[MAX_LEVELS];
};


#endif //RCOMP_TMXFILE_H
