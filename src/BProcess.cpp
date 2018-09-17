#include "BProcess.h"

// #define DEBUG_ME

BProcessList processList;

BProcess::~BProcess() {}

BProcessList::BProcessList() : BListPri() { mResetFlag = ETrue; }

BProcessList::~BProcessList() { Reset(); }

void BProcessList::Reset() {
  BProcess *p;
  while (p = RemHead(), p)
    delete p;
  mResetFlag = ETrue;
}

void BProcessList::AddProcess(BProcess *aProcess) {
  Add(*aProcess);
  mResetFlag = EFalse;
}

void BProcessList::Genocide() {
  BProcess *p = First();
  while (!End(p)) {
    if (p != currentProcess) {
      if (p->Type() != PTYPE_SYSTEM) {
        BProcess *pp = Prev(p);
        p->Remove();
        delete p;
        p = pp;
      }
    }
    p = Next(p);
  }
}

void BProcessList::RunBefore() {
  if (mResetFlag)
    return;
  for (currentProcess = First(); !End(currentProcess);
       currentProcess = Next(currentProcess)) {
    TBool resumeFlag = currentProcess->RunBefore();
    if (mResetFlag)
      return;
    if (!resumeFlag) {
      BProcess *newp = Prev(currentProcess);
      currentProcess->Remove();
      delete currentProcess;
      currentProcess = newp;
    }
  }
}

void BProcessList::RunAfter() {
  if (mResetFlag)
    return;
  for (currentProcess = First(); !End(currentProcess);
       currentProcess = Next(currentProcess)) {
    TBool resumeFlag = currentProcess->RunAfter();
    if (mResetFlag)
      return;
    if (!resumeFlag) {
      BProcess *newp = Prev(currentProcess);
      currentProcess->Remove();
      delete currentProcess;
      currentProcess = newp;
    }
  }
}
