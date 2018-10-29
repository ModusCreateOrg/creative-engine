#include "Game.h"

class GGameProcess : public BProcess {
public:
  GGameProcess() : BProcess() {
    mFont = new BFont(gResourceManager.GetBitmap(FONT_16x16_SLOT), FONT_16x16);
  }

  ~GGameProcess() {
    delete mFont;
  }

  TBool RunBefore() {
    return ETrue;
  }

  TBool RunAfter() {
    mX += 4;

    if (mX >= SCREEN_WIDTH) {
      mX = -224;
      mY += 16;

      if (mY >= SCREEN_HEIGHT) {
        mY = 0;
      }
    }

    gDisplay.renderBitmap->DrawString(ENull, "CreativeEngine", mFont, mX, mY, 0, 1);
    return ETrue;
  }

  TInt mX = 0;
  TInt mY = 0;
  BFont *mFont;
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
