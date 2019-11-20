
#include "../test.h"

#include <BList.h>

static TBool is_ascending(BListPri &list) {
  if (list.End(list.First())) {
    printf("The list is empty!\n");
    return EFalse;
  }

  TInt v = list.First()->pri;
  for (auto *s = list.First(); !list.End(s); s = list.Next(s)) {
    if (s->pri >= v) {
      v = s->pri;
    }
    else {
      return EFalse;
    }
  }
  return ETrue;
}

// BListPri ascending order
static TBool test_001() {
  printf("  Add ascending should create list sorted ascending\n");

  BListPri list;
  BNodePri node1(1), node2(2), node3(3), node4(4), node5(5);

  list.Add(node1);
  list.Add(node5);
  list.Add(node2);
  list.Add(node4);
  list.Add(node3);


  if (list.End(list.First())) {
    printf("The list is empty!\n");
    return EFalse;
  }

  if (!is_ascending(list)) {
    printf("Failed, list is not ascending\n");
    list.Dump();
  }

  list.Reset();
  return ETrue;
}

TBool TestLists() {
  return test_001();
}
