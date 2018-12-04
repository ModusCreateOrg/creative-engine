#include "TBCD.h"

TBCD::TBCD(TUint32 aValue) {
  mValue = aValue;
}

TBCD::TBCD(TBCD &aValue) {
  mValue = aValue.mValue;
}

TBCD::TBCD(const char *aValue) {
  mValue = 0;
  while (*aValue) {
    mValue <<= 4;
    mValue |= *aValue - '0';
  }
}

TBCD::TBCD() {
  mValue = 0;
}

void TBCD::ToString(char aDest[], TBool aZeroPad) {
  char  *pd     = aDest;
  TBool nonZero = EFalse;

  for (TInt i = 0; i < 7; i++) {
    TInt v = (mValue >> ((7 - i) * 4)) & 0x0f;
    if (aZeroPad || v || nonZero) {
      *pd++ = '0' + TInt8(v);
    }
    if (v) {
      nonZero = ETrue;
    }
  }

  TInt v = mValue & 0x0f;
  *pd++  = '0' + TInt8(v);
  *pd    = '\0';
}

void TBCD::Add(TBCD &aValue) {
  TUint32 t1 = mValue + 0x06666666;
  TUint32 t2 = t1 + aValue.mValue;
  TUint32 t3 = t1 ^aValue.mValue;
  TUint32 t4 = t2 ^t3;
  TUint32 t5 = ~t4 & 0x11111110;
  TUint32 t6 = (t5 >> 2) | (t5 >> 3);
  mValue = t2 - t6;
}

void TBCD::FromUint32(TUint32 v) {
  mValue = 0;
  TInt shift = 0;
  while (v > 0) {
    mValue |= (v % 10) << (shift++ << 2);
    v /= 10;
  }
}
