
#include "../test.h"

#include <BList.h>

// BListPri ascending order
bool test_001(){

    BListPri list;
    BNodePri node1(1), node2(2), node3(3), node4(4), node5(5);

    list.Add(node1);
    list.Add(node5);
    list.Add(node2);
    list.Add(node4);
    list.Add(node3);

    if(list.End(list.First())){
        printf("The list is empty!\n");
        return EFalse;
    }

    BNodePri *tail = list.Last();
    for (BNodePri *n = tail->prev; !list.End(n); tail = n, n = n->prev) {
        if(tail->pri < n->pri){
            printf("The list is not ascending! %d, %d\n", tail->pri, n->pri);
            return EFalse;
        }
    }

    list.Reset();
    return ETrue;
}


