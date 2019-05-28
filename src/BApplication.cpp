#include "Display/Display.h"
#include "BApplication.h"

BApplication::BApplication() {
  SeedRandom(300);
  gDisplay.Init();

};

BApplication::~BApplication() {}
