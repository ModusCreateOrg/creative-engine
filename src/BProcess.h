#ifndef BPROCESS_H
#define BPROCESS_H

#include "BList.h"

// process types
// USER is destroyed by Genocide()
// SYSTEM is not destroyed by Genocide()
#define PTYPE_USER 0
#define PTYPE_SYSTEM 1

class BProcess : public BNodePri {
public:
  BProcess(TInt aPri = 0, TInt aType = PTYPE_USER) : BNodePri(aPri) {
    mType = aType;
  }

  virtual ~BProcess();

  TInt Type() { return mType; }

public:
  // Thesefunctions must be provided by inheritor
  // They return EFalse if the process is to be removed from
  // the list and deleted.
  // RunBefore() gets called before Sprites are rendered
  // RunAfter() gets called after Sprites are rendered
  virtual TBool RunBefore() = 0;

  virtual TBool RunAfter() = 0;

protected:
  TInt mType;
};

class BProcessList : public BListPri {
public:
  BProcessList();

  virtual ~BProcessList();

  void Reset();

  void AddProcess(BProcess *aProcess);

  void Genocide();

public:
  BProcess *RemHead() { return (BProcess *) BListPri::RemHead(); }

  BProcess *First() { return (BProcess *) next; }

  BProcess *Next(BProcess *curr) { return (BProcess *) curr->next; }

  BProcess *Last() { return (BProcess *) prev; }

  BProcess *Prev(BProcess *curr) { return (BProcess *) curr->prev; }

  TBool End(BProcess *curr) { return curr == (BProcess *) this; }

public:
  void RunBefore();

  void RunAfter();

protected:
  TBool    mResetFlag; // true if Reset() has been called
  BProcess *mCurrentProcess;
};

// extern BProcessList gProcessList;

#endif
