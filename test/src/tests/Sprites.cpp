#include "../test.h"

#include <BSprite.h>
#include "../GResources.h"
#include <BResourceManager.h>

// BSpriteList ascending order

void print_list(BSpriteList &list) {
  for (auto *s = list.First(); !list.End(s); s = list.Next(s)) {
    printf("Node %p PRI(%d)\n", s, s->pri);
  }
}

static TBool is_ascending(BSpriteList &list) {
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

static BSprite *make_sprite(TInt pri) {
  auto *sprite = new BSprite(pri, SPLASH_SLOT);
  sprite->SetFlags(SFLAG_SORTPRI);
}

static void make_ascending(BSpriteList &list) {
  auto *sprite1 = make_sprite(10);
  auto *sprite2 = make_sprite(20);
  auto *sprite3 = make_sprite(30);
  auto *sprite4 = make_sprite(40);
  auto *sprite5 = make_sprite(50);
  auto *sprite6 = make_sprite(60);
  auto *sprite7 = make_sprite(70);

  list.Add(*sprite1);
  list.Add(*sprite2);
  list.Add(*sprite3);
  list.Add(*sprite4);
  list.Add(*sprite5);
  list.Add(*sprite6);
  list.Add(*sprite7);
}

static void make_descending(BSpriteList &list) {
  auto *sprite1 = make_sprite(10);
  auto *sprite2 = make_sprite(20);
  auto *sprite3 = make_sprite(30);
  auto *sprite4 = make_sprite(40);
  auto *sprite5 = make_sprite(50);
  auto *sprite6 = make_sprite(60);
  auto *sprite7 = make_sprite(70);

  list.Add(*sprite7);
  list.Add(*sprite6);
  list.Add(*sprite5);
  list.Add(*sprite4);
  list.Add(*sprite3);
  list.Add(*sprite2);
  list.Add(*sprite1);
}

static TBool test_001() {
  printf("  Add sprites ascending, list should be ascending");

  BSpriteList list;
  make_ascending(list);

  if (!is_ascending(list)) {
    return EFalse;
  }

  return ETrue;
}

static TBool test_002() {
  printf("  Add sprites descending, list should be ascending");

  BSpriteList list;
  make_descending(list);

  if (!is_ascending(list)) {
    return EFalse;
  }

  return ETrue;
}

static TBool test_003() {
  printf("  BSpriteList::Move should keep list sorted ascending by pri");
  BSpriteList list;
  make_descending(list);

  if (!is_ascending(list)) {
    return EFalse;
  }

  list.Move();
  if (!is_ascending(list)) {
    print_list(list);
    return EFalse;
  }
  return ETrue;

}

TBool run(test_func_t func) {
  fflush(stdout);
  if (func()) {
    printf(" PASSED!\n");
    fflush(stdout);
    return ETrue;
  }
  else{
    printf(" FAILED!\n");
    fflush(stdout);
    return EFalse;
  }
}

TBool TestSprites() {
  return run(test_001) && run(test_002) && run(test_003);
}