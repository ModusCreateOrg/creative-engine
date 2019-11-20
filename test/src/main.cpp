#include "test.h"
#include <stdio.h>
#include <BResourceManager.h>
#include "./GResources.h"

test_t tests[] = {
  {TestLists,   "Test Lists"},
  {TestSprites, "Test Sprites"},
  {0,           0}
};

int main() {
  // preload a bitmap into a slot for test purposes
  gResourceManager.LoadBitmap(SPLASH_SPRITES_BMP, SPLASH_SLOT, IMAGE_16x16);
  test_t *tst = tests;
  int cnt = 0;
  while (tst->func && tst->label) {
    cnt++;
    printf("%s\n", tst->label);
    if ((*tst->func)()) {
      printf("  == %s - PASSED\n", tst->label);
    } else {
      printf("  == %s - FAILED\n", tst->label);
      return 1;
    }
    tst++;
  }

  printf("\nALL TESTS PASSED");
  return 0;
}
