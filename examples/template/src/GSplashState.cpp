#include "Game.h"
#include "cstring"

class GSplashProcess : public BProcess {
public:
  GSplashProcess() : BProcess() {}

public:
  TBool RunBefore() {
    return ETrue;
  }

  TBool RunAfter() {
    if (gControls.WasPressed(BUTTON_ANY)) {
      gGame->SetState(GAME_STATE_GAME);
      return EFalse;
    }
    return ETrue;
  }
};

class GSplashPlayfield : public BPlayfield {
public:
  GSplashPlayfield() {
    // Init fonts
    mFont8 = new BFont(gResourceManager.GetBitmap(FONT_8x8_SLOT), FONT_8x8);
    mFont16 = new BFont(gResourceManager.GetBitmap(FONT_16x16_SLOT), FONT_16x16);

    // Text X coords
    mTitleX = (SCREEN_WIDTH - strlen(mTitle) * mFont16->mWidth) / 2;
    mSubTitleX = (SCREEN_WIDTH - strlen(mSubTitle) * mFont16->mWidth) / 2;
    mInitTextX = (SCREEN_WIDTH - strlen(mInitText) * mFont8->mWidth) / 2;

    gDisplay.SetPalette(gResourceManager.GetBitmap(FONT_8x8_SLOT));
  }

  ~GSplashPlayfield() {
    delete mFont8;
    delete mFont16;
  }

  void Render() {
    gDisplay.renderBitmap->Clear();
    gDisplay.renderBitmap->DrawString(ENull, mTitle, mFont16, mTitleX, 70, 1, 0);
    gDisplay.renderBitmap->DrawString(ENull, mSubTitle, mFont16, mSubTitleX, 110, 0, 1);

    if (mTimer-- > 0) {
      gDisplay.renderBitmap->DrawString(ENull, mInitText, mFont8, mInitTextX, 170, 1, 0);
      return;
    }

    if (mDelay-- > 0) {
      return;
    }

    mTimer = 20;
    mDelay = 10;
  }

  TInt mTimer = 20;
  TInt mDelay = 10;
  const char *mTitle = "CreativeEngine";
  const char *mSubTitle = "{Stub} Example";
  const char *mInitText = "Press any button";
  TInt mTitleX;
  TInt mSubTitleX;
  TInt mInitTextX;
  BFont *mFont8;
  BFont *mFont16;
};

GSplashState::GSplashState() : BGameEngine(gViewPort) {
  mPlayfield = new GSplashPlayfield();
  auto *p = new GSplashProcess();
  AddProcess(p);
}
