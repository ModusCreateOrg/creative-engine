#ifndef GENUS_TNUMBER_H
#define GENUS_TNUMBER_H

#include "BTypes.h"
#include "cmath"
#include <strings.h>

struct TPad {
  const char c;
  const TUint count;
};

struct TNumber {
  TInt  mVal;
  TUint mPrec;

  TNumber() {
    mVal = 0;
    mPrec = 0;
  }

  TNumber(TInt aVal, TUint aPrec = 0) {
    mVal = aVal;
    mPrec = aPrec;
  }

  TFloat ToFloat() {
    return mVal / TFloat(mPrec);
  }

  TInt ToInt() {
    return mVal / mPrec;
  }

  char* ToString(char *c, const TPad *aPad) {
    TBool isFloat = mPrec > 0;
    TBool isPadded = aPad && aPad->count;
    TFloat fl = mVal / TFloat(mPrec);
    TUint whole = TUint(fl);

    // Count number of digits in whole part
    TUint digitCount = 0;
    TUint decimalCount = 0;
    TUint n = whole;
    do {
      digitCount++;
      n /= 10;
    } while (n);

    if (isFloat) {
      // Count number of decimals (incl separator)
      n = mPrec;
      do {
        decimalCount++;
        n /= 10;
      } while (n);

      // Adjust padding for decimals and separator
      if (aPad->count > decimalCount) {
        digitCount = MAX(aPad->count - decimalCount, digitCount);
      }
    } else if(isPadded) {
      // Set padding
      digitCount = MAX(aPad->count, digitCount);
    }

    // Fill char array with pad chars
    const char fillChar = isPadded ? aPad->c : '0';
    TUint i = 0;
    for (; i < digitCount; i++) {
      c[i] = fillChar;
    }
    // Required for strlen to function across platforms
    c[i] = '\0';

    // Store current string length
    // Store a copy of strlen for writing the whole part right to left
    TUint valueStrLen = strlen(c);
    TUint valueStrLenTmp = valueStrLen;
    n = whole;
    c[--valueStrLenTmp] = '0' + n % 10;
    while(n >= 10) {
      n /= 10;
      c[--valueStrLenTmp] = '0' + n % 10;
    }

    if (isFloat) {
      // There are more than 1 decimal places
      if (mPrec > 10) {
        n = mPrec;
        c[valueStrLen] = '.';

        // Move the starting point for writing decimals from right to left
        while (n) {
          valueStrLen++;
          n /= 10;
        }

        // Write decimals right to left
        // starting from \0, until we hit the . separator
        TUint d = mVal % mPrec;
        c[valueStrLen] = '\0';
        c[--valueStrLen] = '0' + d % 10;
        while(c[valueStrLen - 1] != '.') {
          d /= 10;
          c[--valueStrLen] = '0' + d % 10;
        }
      } else {
        // Write separator, decimal and \0
        c[valueStrLen++] = '.';
        c[valueStrLen++] = '0' + mVal % mPrec;
        c[valueStrLen] = '\0';
      }
    }

    return c;
  }
};

#endif //GENUS_TNUMBER_H
