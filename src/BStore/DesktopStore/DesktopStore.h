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

public:
  char mTargetDir[4096];

public:
  void Initialize() OVERRIDE {
    char *homeDir = getenv("HOME");

    strcpy(mTargetDir, homeDir);
    strcat(mTargetDir, "/.modus");

    DIR *dir = opendir(mTargetDir);
    if (dir) {
      closedir(dir);
    }
    else {
      mkdir(mTargetDir, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH);
    }

    mInitialized = ETrue;
  }

  TBool Get(const char *aKey, void *aValue, TUint32 aSize) override {
    char name[4096];
    sprintf(name, "%s/%s.%s.store", mTargetDir, mStoreName, aKey);

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
    sprintf(name, "%s/%s.%s.store", mTargetDir, mStoreName, aKey);

    FILE *fp = fopen(name, "w");
    if (!fp || fwrite(aValue, aSize, 1, fp) != aSize) {
      fclose(fp);
      return EFalse;
    }
    fclose(fp);
    return ETrue;
  }

  TBool Remove(const char *aKey) OVERRIDE {
    char name[4096];
    sprintf(name, "%s/%s.%s.store", mTargetDir, mStoreName, aKey);
    return unlink(name) == 0;
  }
};

#endif //GENUS_DESKTOPSTORE_H
