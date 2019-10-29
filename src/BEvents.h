#ifndef BEVENTS_H
#define BEVENTS_H

// Events / Event Emitte

#include "BBase.h"
#include "BList.h"

class BProcess;

// Event Listener Node
class BEventListener : public BNode {
public:
  BEventListener(TUint16 aType, BProcess *aProcess) : mType(aType), mProcess(aProcess) {}

public:
  TUint16 mType;
  BProcess *mProcess; // process that will listen
};

// list of Event Listeners
class BEventListenerList : public BList {
public:
  BEventListenerList() : BList() {}
  virtual ~BEventListenerList() {}

public:
  //  void AddTail(BEventListener *listener) { AddTail((BNode *)listener); }

  BEventListener *First() { return (BEventListener *)next; }

  BEventListener *Next(BEventListener *curr) { return (BEventListener *)curr->next; }

  BEventListener *Last() { return (BEventListener *)prev; }

  BEventListener *Prev(BEventListener *curr) { return (BEventListener *)curr->prev; }

  TBool End(BEventListener *curr) { return curr == (BEventListener *)this; }
};

// Event Message Node
class BEventMessage : public BNode {
public:
  BEventMessage(TUint16 aType, BProcess *aSender, TAny *aMessage) : mType(aType), mSender(aSender), mMessage(aMessage) {}

public:
  TUint16 mType;
  BProcess *mSender;
  TAny *mMessage;
};

class BEventMessageList : public BList {
public:
  BEventMessageList() : BList() {}
  virtual ~BEventMessageList() {}

public:
  //  void AddTail(BEventMessageList *listener) { AddTail((BNode *)listener); }

  BEventMessageList *First() { return (BEventMessageList *)next; }

  BEventMessageList *Next(BEventMessageList *curr) { return (BEventMessageList *)curr->next; }

  BEventMessageList *Last() { return (BEventMessageList *)prev; }

  BEventMessageList *Prev(BEventMessageList *curr) { return (BEventMessageList *)curr->prev; }

  TBool End(BEventMessageList *curr) { return curr == (BEventMessageList *)this; }
};

// The Event Emitter
class BEventEmitter : public BBase {
//public:
//  BEventEmitter() {}
//  ~BEventEmitter() {}

public:
  TBool Listen(TUint16 type, BProcess *aProcess);
  TBool UnListen(TUint16 type, BProcess *aProcess);
  void UnListenAll(BProcess *aProcess);

  // Trigger/fire an event
  TBool FireEvent(BProcess *aSender, TUint16 aType, TAny *aMessage);

protected:
  BEventListenerList mListeners;
};

extern BEventEmitter gEventEmitter;

#endif
