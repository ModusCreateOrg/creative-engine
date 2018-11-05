#ifndef GENUS_TBCD_H
#define GENUS_TBCD_H

#include "BTypes.h"

// useful for scores and other numbers printed on screen
struct TBCD {
  TBCD(TUint32 aValue);

  TBCD(TBCD &aValue);

  TBCD(const char *aValue);

  TBCD();

public:
  void ToString(char aDest[], TBool aZeroPad = ETrue);

  void Add(TBCD &aValue);

  void FromUint32(TUint32 v);

  TUint32 operator*() {
    return mValue;
  }

  TUint32 mValue;
};


#endif //GENUS_TBCD_H
