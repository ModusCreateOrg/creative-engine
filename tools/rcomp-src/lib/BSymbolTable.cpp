#include "BSymbolTable.h"

// hash a string to a value between 0-255
static TInt16 hash(const char *s) {
  // this routine is stupid simple.  There are other hash algorithms that produce a better distribution of hash values.
  // better distribution makes it so one bucket list doesn't get huge while others are empty.
  // better distribution probably comes at a CPU processing cost, though.
  // for our purposes, this is fine.
  TInt v = 0;
  while (*s != '\0') {
    v += *s;
    s++;
  }
  return v%256;
}

BSymbolTable::BSymbolTable() {
  //
}

BSymbolTable::~BSymbolTable() {
  //
}

BSymbol *BSymbolTable::LookupSymbol(const char *name) {
  TInt h = hash(name);
  BSymbolList &bucket = buckets[h];
  for (BSymbol *sym = bucket.First(); !bucket.End(sym); sym=bucket.Next(sym)) {
    if (strcmp(sym->name, name) == 0) {
      return sym;
    }
  }
  return ENull;
}

TBool BSymbolTable::AddSymbol(const char *aName, TUint32 aValue, TAny *aPtr) {
  BSymbol *sym = LookupSymbol(aName);
  if (sym) {
    // already exists
    if (sym->value == aValue && sym->aptr == aPtr) {
      // trying to add a duplicate, pretend it succeeded)
      return ETrue;
    }
    else {
      // trying to add same name with different value!
      return EFalse;
    }
  }
  // doesn't exist, we'll add it
  TInt h = hash(aName);
  sym = new BSymbol(aName, aValue, aPtr);
  buckets[h].AddTail(*sym);
  return ETrue;
}

