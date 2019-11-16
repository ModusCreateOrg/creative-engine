#include "test.h"
#include <stdio.h>
#include <BResourceManager.h>

test_t tests[] = {
        {test_001, "BListPri ascending order"},
        {test_002, "BSpriteList ascending order"},
        {0,        0}
};

int main(){

    test_t *tst = tests;
    int cnt = 0;
    while(tst->func && tst->label){
        cnt++;
        if((*tst->func)()){
            printf("test_%03d: %s - PASSED\n", cnt, tst->label);
        }
        else{
            printf("test_%03d: %s - FAILED\n", cnt, tst->label);
            return 1;
        }
        tst++;
    }

  printf("ALL TESTS PASSED");
  return 0;
}
