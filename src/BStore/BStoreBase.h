#ifndef GENUS_BSTOREBASE_H
#define GENUS_BSTOREBASE_H



#include "BTypes.h"
#include "BBase.h"
#include <string.h>
#include <stdlib.h>
#include <string.h>

class BStoreBase : public BBase {
public:
  explicit BStoreBase(const char *aStoreName) : mStoreName(strdup(aStoreName)) {
    mInitialized = EFalse;
  };

  virtual void Initialize() = 0;

  ~BStoreBase() {
    FreeMem((TAny*)mStoreName);
  };

public:
  // Get a blob of data named by key into aValue, of given size
  virtual TBool Get(const char *aKey, void *aValue, TUint32 aSize) = 0;

  // Set a blob of data named by key to value of given size
  virtual TBool Set(const char *aKey, void *aValue, TUint32 aSize) = 0;

  // Remove a blob of data named by key 
  virtual TBool Remove(const char *aKey) = 0;
public:
  const char *mStoreName;

protected:
  static TBool mInitialized;
};


#endif //GENUS_BSTOREBASE_H
