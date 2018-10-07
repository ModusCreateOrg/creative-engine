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
 * Double linked list of elements. Elements are simply added to the head or tail of the list.
 */
class BList : public BNode {
public:
    BList();
    virtual ~BList();

  /**
   * Remove all of the elements from the list.
   */
    virtual void Reset() {
    next = (BNode *)this;
    prev = (BNode *)this;
  }

  virtual void AddHead(BNode &node);
  virtual BNode *RemHead();
  virtual void AddTail(BNode &node);
  virtual BNode *RemTail();
  virtual void RemoveNode(BNode *node);
  /**
  * Get the element from the head of the list.
  * @return The head element.
  */
  virtual BNode *First() { return next; }
  /**
   * Get the element after the specified element.
   * @param curr The current element;
   * @return The next element in the list;
   */
  virtual BNode *Next(BNode *curr) { return curr->next; }
  /**
    * Get the element from the tail of the list.
    * @return The head element.
    */
  virtual BNode *Last() { return prev; }
  /**
   * Get element before the specified element.
   * @param curr The current element;
   * @return The previous element in the list;
   */
  virtual BNode *Prev(BNode *curr) { return curr->prev; }
  /**
   * Test if the specified element is the end of the list.
   * @param curr The element to test.
   * @return True if is is the end or faes if it is not the end.
   */
    virtual TBool End(BNode *curr) { return curr == (BNode *)this; }
};

/**
 * Double linked list of elements, sorted by descending priority.
 *
 * Elements with the highest priority value are near the head of the list.
 * Elements with the lowest priority value are near the tail of the list.
 */
class BListPri : public BNodePri {
public:
  BListPri();
  virtual ~BListPri();

public:
  /**
   * Remove all of the elements from the list.
   */
  virtual void Reset() {
    next = (BNodePri *)this;
    prev = (BNodePri *)this;
  }

  virtual void Add(BNodePri &node);
  virtual void RemoveNode(BNodePri *node);
  /**
   * Get the head element from the list.
   * @return The head element.
   */
  virtual BNodePri *First() { return next; }
  /**
   * Test if the specified element is the end of the list.
   * @param curr The element to test.
   * @return True if is is the end or faes if it is not the end.
   */
  virtual TBool End(BNodePri *curr) { return curr == (BNodePri *)this; }
  virtual BNodePri *RemHead();
  virtual BNodePri *RemTail();
  /**
   * Get the element after the specified element.
   * @param curr The current element;
   * @return The next element in the list;
   */
  virtual BNodePri *Next(BNodePri *curr) { return curr->next; }
  /**
    * Get the element from the tail of the list.
    * @return The head element.
    */
  virtual BNodePri *Last() { return prev; }
  /**
   * Get element before the specified element.
   * @param curr The current element;
   * @return The previous element in the list;
   */
  virtual BNodePri *Prev(BNodePri *curr) { return curr->prev; }

private:
  virtual void AddHead(BNodePri &nodevirtual );
  virtual void AddTail(BNodePri &node);


};

#endif
