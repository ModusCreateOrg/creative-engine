#ifndef RCOMP_BSYMBOLTABLE_H
#define RCOMP_BSYMBOLTABLE_H

#include "BList.h"

const TInt HASH_SIZE = 256;

struct BSymbol : public BNode {
  BSymbol(const char *aName, TUint32 aValue, void *aPtr = ENull) {
    name = strdup(aName);
    value = aValue;
    aptr = aPtr;
  }

  ~BSymbol() {
    delete[] name;
  }

  char *name;
  void *aptr; // ptr to anything you want to be able to lookup
  TUint32 value;
};

struct BSymbolList : public BList {
public:
  ~BSymbolList() {
    while (BSymbol *s = RemHead()) {
      delete s;
    }
  }

public:
  BSymbol *RemHead() { return (BSymbol *) BList::RemHead(); }

  BSymbol *First() { return (BSymbol *) next; }

  BSymbol *Next(BSymbol *curr) { return (BSymbol *) curr->next; }

  BSymbol *Last() { return (BSymbol *) prev; }

  BSymbol *Prev(BSymbol *curr) { return (BSymbol *) curr->prev; }

  TBool End(BSymbol *curr) { return curr == (BSymbol *) this; }
};

class BSymbolTable {
public:
  BSymbolTable();

  ~BSymbolTable();

public:
  BSymbol *LookupSymbol(const char *name);

  TBool AddSymbol(const char *aName, TUint32 aValue, TAny *aPtr = ENull);

protected:
  BSymbolList buckets[HASH_SIZE];
};


#endif //RCOMP_BSYMBOLTABLE_H
