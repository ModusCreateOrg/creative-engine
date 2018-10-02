#include "BProcess.h"

// #define DEBUG_ME

BProcess::~BProcess() {}

BProcessList::BProcessList() : BListPri() {
  mResetFlag = ETrue;
}

BProcessList::~BProcessList() { Reset(); }

void BProcessList::Reset() {
  BProcess *p;
  while (p   = RemHead(), p)
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
    if (p != mCurrentProcess) {
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
  for (mCurrentProcess = First(); !End(mCurrentProcess);
       mCurrentProcess = Next(mCurrentProcess)) {
    TBool resumeFlag = mCurrentProcess->RunBefore();
    if (mResetFlag)
      return;
    if (!resumeFlag) {
      BProcess *newp = Prev(mCurrentProcess);
      mCurrentProcess->Remove();
      delete mCurrentProcess;
      mCurrentProcess = newp;
    }
  }
  mCurrentProcess = ENull;
}

void BProcessList::RunAfter() {
  if (mResetFlag)
    return;
  for (mCurrentProcess = First(); !End(mCurrentProcess);
       mCurrentProcess = Next(mCurrentProcess)) {
    TBool resumeFlag = mCurrentProcess->RunAfter();
    if (mResetFlag)
      return;
    if (!resumeFlag) {
      BProcess *newp = Prev(mCurrentProcess);
      mCurrentProcess->Remove();
      delete mCurrentProcess;
      mCurrentProcess = newp;
    }
  }
  mCurrentProcess = ENull;
}
