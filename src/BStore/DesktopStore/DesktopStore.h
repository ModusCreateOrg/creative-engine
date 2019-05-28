#ifndef GENUS_DESKTOPSTORE_H
#define GENUS_DESKTOPSTORE_H

#include <dirent.h>
#include <sys/stat.h>

#include "BStoreBase.h"


class DesktopStore : public BStoreBase {
public:
  explicit DesktopStore(const char *aStoreName) : BStoreBase(aStoreName) {
    Initialize();
  }

  void Initialize() override {
    char *homeDir = getenv("HOME");

    char targetDir[4096];
    strcpy(targetDir, homeDir);
    strcat(targetDir, "/.modus");

    DIR* dir = opendir(targetDir);
    if (dir) {
      closedir(dir);
    }
    else {
      mkdir(targetDir, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH);
    }

    mInitialized = ETrue;
  }



  TBool Get(const char *aKey, void *aValue, TUint32 aSize) override {
    char name[4096];
    char *homeDir = getenv("HOME");

    strcpy(name, homeDir);
    strcat(name, "/.modus/");
    strcat(name, mStoreName);
    strcat(name, ".");
    strcat(name, aKey);
    strcat(name, ".store");

    FILE *fp = fopen(name, "r");
    if (!fp) {
      return EFalse;
    }
    if (fread(aValue, 1, aSize, fp) != aSize) {
      fclose(fp);
      return EFalse;
    }
    fclose(fp);
    return ETrue;
  }

  TBool Set(const char *aKey, void *aValue, TUint32 aSize) override {
    char name[4096];
    char *homeDir = getenv("HOME");

    strcpy(name, homeDir);
    strcat(name, "/.modus/");
    strcat(name, mStoreName);
    strcat(name, ".");
    strcat(name, aKey);
    strcat(name, ".store");

    FILE *fp = fopen(name, "w");
    if (!fp || fwrite(aValue, aSize, 1, fp) != aSize) {
      fclose(fp);
      return EFalse;
    }
    fclose(fp);
    return ETrue;
  }

};


#endif //GENUS_DESKTOPSTORE_H
