#ifndef GENUS_BSTORE_H
#define GENUS_BSTORE_H

#include "BTypes.h"
#include "BBase.h"

class BStore : public BBase {
public:
  BStore(const char *aStoreName);

  ~BStore();

public:
  // Get a blob of data named by key into aValue, of given size
  TBool Get(const char *aKey, void *aValue, TUint32 aSize);

  // Set a blob of data named by key to value of given size
  TBool Set(const char *aKey, void *aValue, TUint32 aSize);

public:
  const char *mStoreName;
protected:
  static TBool mInitialized;
};


#endif //GENUS_BSTORE_H
