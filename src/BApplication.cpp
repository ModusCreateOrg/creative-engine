#include "Display/Display.h"
#include "BApplication.h"

BApplication::BApplication() {
  SeedRandom(time(ENull));
  gDisplay.Init();

};

BApplication::~BApplication() {}

static void Exit(TInt aExitCode) {
  exit(aExitCode);
}
