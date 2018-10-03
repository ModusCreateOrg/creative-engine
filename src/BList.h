#ifndef BLIST_H
#define BLIST_H

#include "BBase.h"

/**
 * Two basic types of doubly linked lists:
 *
 * 1) BList is a linked list of BNodes
 * 2) BListPri is a linked list of BNodePri nodes, sorted by Priority.
 */

/**
 * An element in a BList linked list .
 *
 * You will inherit from this to add useful member variables - a list of BNodes
 * isn't very interesting.
 */
class BNode : public BBase {
public:
  BNode();
  virtual ~BNode();

public:
  // make this node last on the list, if node is key
  void InsertBeforeNode(BNode *nnode) {
    BNode *pnode = nnode->prev;
    pnode->next = this;
    nnode->prev = this;
    next = nnode;
    prev = pnode;
  }
  // make this node first on the list, if node is key
  void InsertAfterNode(BNode *pnode) {
    BNode *nnode = pnode->next;
    pnode->next = this;
    nnode->prev = this;
    next = nnode;
    prev = pnode;
  }
  void Remove() {
    next->prev = prev;
    prev->next = next;
  }

public:
  BNode *next, *prev;
};

/**
 * An element in a BListPri linked list.
 */
class BNodePri : public BBase {
public:
  BNodePri(TInt aPri);
  virtual ~BNodePri();

public:
  void InsertBeforeNode(BNodePri *nnode) {
    BNodePri *pnode = nnode->prev;
    pnode->next = this;
    nnode->prev = this;
    next = nnode;
    prev = pnode;
  }
  // make this node first on the list, if node is key
  void InsertAfterNode(BNodePri *pnode) {
    BNodePri *nnode = pnode->next;
    pnode->next = this;
    nnode->prev = this;
    next = nnode;
    prev = pnode;
  }
  void Remove() {
    next->prev = prev;
    prev->next = next;
  }

public:
  BNodePri *next, *prev;
  TInt pri;
};

/**
 * A simple double linked list of BNodes.
 */
class BList : public BNode {
public:
  BList();
  virtual ~BList();

public:
  virtual void Reset() {
    next = (BNode *)this;
    prev = (BNode *)this;
  }

  virtual void AddHead(BNode &node);
  virtual BNode *RemHead();
  virtual void AddTail(BNode &node);
  virtual BNode *RemTail();
  virtual void RemoveNode(BNode *node);

  virtual BNode *First() { return next; }
  virtual BNode *Next(BNode *curr) { return curr->next; }
  virtual BNode *Last() { return prev; }
  virtual BNode *Prev(BNode *curr) { return curr->prev; }
  virtual TBool End(BNode *curr) { return curr == (BNode *)this; }
};

/**
 * A double linked list of BNodePri, sorted by Priority.
 */
class BListPri : public BNodePri {
public:
  BListPri();
  virtual ~BListPri();

public:
  virtual void Reset() {
    next = (BNodePri *)this;
    prev = (BNodePri *)this;
  }

  virtual void Add(BNodePri &node);
  virtual void AddHead(BNodePri &nodevirtual );
  virtual BNodePri *RemHead();
  virtual void AddTail(BNodePri &node);
  virtual BNodePri *RemTail();
  virtual void RemoveNode(BNodePri *node);

  virtual BNodePri *First() { return next; }
  virtual BNodePri *Next(BNodePri *curr) { return curr->next; }
  virtual BNodePri *Last() { return prev; }
  virtual BNodePri *Prev(BNodePri *curr) { return curr->prev; }
  virtual TBool End(BNodePri *curr) { return curr == (BNodePri *)this; }
};

#endif
