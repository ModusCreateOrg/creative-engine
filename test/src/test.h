#ifndef TEST_H
#define TEST_H

#include <BTypes.h>

typedef bool (*test_func_t)();

typedef struct{
    test_func_t func;
    const char* label;
} test_t;

bool test_001();
bool test_002();

#endif