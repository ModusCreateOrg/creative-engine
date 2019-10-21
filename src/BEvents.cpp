#include "BEvents.h"
#include "BProcess.h"

// Events / Event Emitter
BEventEmitter gEventEmitter;

TBool BEventEmitter::Listen(TUint16 aType, BProcess *aProcess) {
  for (BEventListener *l = mListeners.First(); !mListeners.End(l); l = mListeners.Next(l)) {
    if (l->mType == aType && l->mProcess == aProcess) {
      return EFalse;
    }
  }
  BEventListener *n = new BEventListener(aType, aProcess);
  mListeners.AddTail(*n);
  return ETrue;
}

TBool BEventEmitter::UnListen(TUint16 aType, BProcess *aProcess) {
  for (BEventListener *l = mListeners.First(); !mListeners.End(l); l = mListeners.Next(l)) {
    if (l->mType == aType && l->mProcess == aProcess) {
      l->Remove();
      delete l;
      return ETrue;
    }
  }
  return EFalse;
}

void BEventEmitter::UnListenAll(BProcess *aProcess) {
  for (BEventListener *l = mListeners.First(); !mListeners.End(l);) {
    BEventListener *next_l = mListeners.Next(l);
    if (l->mProcess == aProcess) {
      l->Remove();
      delete l;
    }
    l = next_l;
  }
}

TBool BEventEmitter::FireEvent(BProcess *aSender, TUint16 aType, TAny *aMessage) {
  TBool sent = EFalse;
  for (BEventListener *l = mListeners.First(); !mListeners.End(l); l = mListeners.Next(l)) {
    if (l->mType == aType && l->mProcess != aSender) {
      BEventMessage *m = new BEventMessage(aType, aSender, aMessage);
      l->mProcess->ReceiveMessage(m);
      sent = ETrue;
    }
  }
  return sent;
}
