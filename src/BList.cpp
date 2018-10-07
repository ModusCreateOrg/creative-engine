#include "BList.h"

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/// BNode

BNode::BNode() : BBase() {}

BNode::~BNode() {}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/// BNodePri

BNodePri::BNodePri(TInt aPri) : BBase() { pri = aPri; }

BNodePri::~BNodePri() {}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/// BList

/**
 * Create an empty list.
 */
BList::BList() : BNode() { Reset(); }
/**
 * Print a message if the list is not empty when it is deleted.
 */
BList::~BList() {
  //  if (next != this) printf("List not empty!");
}

/**
 * Add an element to the tail of the list.
 * @param node The element to add.
 */
void BList::AddTail(BNode &node) { node.InsertBeforeNode(this); }

/**
 * Add an element to the head of the list.
 * @param node The element to add.
 */
void BList::AddHead(BNode &node) { node.InsertAfterNode(this); }

/**
 * Remove the element at the head of the list.
 * @return The removed element.
 */
BNode *BList::RemHead() {
  BNode *n = next;
  if (n == (BNode *)this)
    return NULL;
  n->Remove();
  return n;
}

/**
 * Remove the element at the tail of the list.
 * @return The removed element.
 */
BNode *BList::RemTail() {
  BNode *n = prev;
  if (n == (BNode *)this)
    return NULL;
  n->Remove();
  return n;
}

/**
 * Remove the specified element from the list.
 *  * @param node The element to remove.
 */
void BList::RemoveNode(BNode *node) { node->Remove(); }

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/// BListPri

/**
 * Create an empty priority list.
 */
BListPri::BListPri() : BNodePri(0) {
  Reset();
}

/**
 * Print a message if the list is not empty when it is deleted.
 */
BListPri::~BListPri() {
  //  if (next != this) printf("List not empty!");
}

void BListPri::AddTail(BNodePri &node) { node.InsertBeforeNode(this); }

void BListPri::AddHead(BNodePri &node) { node.InsertAfterNode(this); }

/**
 * Remove the prioritized element at the head of the list.
 * @return The removed element.
 */
BNodePri *BListPri::RemHead() {
  BNodePri *n = next;
  if (n == (BNodePri *)this)
    return NULL;
  n->Remove();
  return n;
}

/**
 * Remove the prioritized element at the tail of the list.
 * @return The removed element.
 */
BNodePri *BListPri::RemTail() {
  BNodePri *n = prev;
  if (n == (BNodePri *)this)
    return NULL;
  n->Remove();
  return n;
}

/**
 * Remove the specified element from the list.
 * @param node The element to remove.
 */
void BListPri::RemoveNode(BNodePri *node) { node->Remove(); }

void BListPri::Add(BNodePri &node) {
  for (BNodePri *n = First(); !End(n); n = n->next) {
    if (node.pri >= n->pri) {
      node.InsertBeforeNode(n);
      return;
    }
  }
  AddTail(node);
}
