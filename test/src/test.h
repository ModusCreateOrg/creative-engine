#ifndef TEST_H
#define TEST_H

#include <BTypes.h>

typedef TBool (*test_func_t)();

typedef struct{
    test_func_t func;
    const char* label;
} test_t;

extern TBool TestLists();
extern TBool TestSprites();

#endif