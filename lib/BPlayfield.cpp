#include "BPlayfield.h"
#include "BGameEngine.h"

BPlayfield::BPlayfield(BGameEngine *aGameEngine) {
  mGameEngine = aGameEngine;
  mDisplay = aGameEngine->mDisplay;
}

BPlayfield::~BPlayfield() {
}

void BPlayfield::Animate() {
}