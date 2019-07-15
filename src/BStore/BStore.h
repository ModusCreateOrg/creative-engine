#ifndef GENUS_BSTORE_H
#define GENUS_BSTORE_H

// ODROID GO
#ifdef __XTENSA__
#include "OdroidStore/OdroidStore.h"
class BStore : public OdroidStore {
public:
  explicit BStore(const char *aStoreName) : OdroidStore(aStoreName)  {};
  ~BStore() {

  }
};
#endif


#ifndef __XTENSA__
#include "DesktopStore.h"
class BStore : public DesktopStore {
public:
  explicit BStore(const char *aStoreName) : DesktopStore(aStoreName)  {};
  ~BStore() {

  }
};
#endif

#endif //GENUS_BSTORE_H
