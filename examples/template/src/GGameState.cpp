#include "Game.h"

class GGameProcess : public BProcess {
public:
  GGameProcess() : BProcess() {}
  ~GGameProcess() {}

  TBool RunBefore() {
    return ETrue;
  }

  TBool RunAfter() {
    return ETrue;
  }
};

class GGamePlayfield : public BPlayfield {
public:
  GGamePlayfield() {
    gDisplay.SetPalette(gResourceManager.GetBitmap(FONT_8x8_SLOT));
  }

  void Render() {
    gDisplay.renderBitmap->Clear();
  }
};

GGameState::GGameState() : BGameEngine(gViewPort) {
  mPlayfield = new GGamePlayfield();
  auto *p = new GGameProcess();
  AddProcess(p);
}
