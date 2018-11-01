#include "Game.h"
#include "cmath"

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

    // Clipping
    gDisplay.renderBitmap->DrawString(ENull, "CreativeEngine", mFont, mX, mY, 0, 1);

    // Letter spacing
    if (mLetterSpacing == 4.0) {
      mIncrement = EFalse;
    } else if (mLetterSpacing == -4.0) {
      mIncrement = ETrue;
    }

    const char *str = "Modus Create";
    mLetterSpacing += mIncrement ? 0.25 : -0.25;
    TInt centerX = (SCREEN_WIDTH - strlen(str) * mFont->mWidth) / 2;
    TInt centerY = (SCREEN_HEIGHT - mFont->mHeight) / 2;

    gDisplay.renderBitmap->DrawString(ENull, str, mFont, centerX, centerY, 1, -1, TInt(round(mLetterSpacing)));

    return ETrue;
  }

  TInt mX = 0;
  TInt mY = 0;
  TFloat mLetterSpacing = 0;
  TBool mIncrement = ETrue;
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
