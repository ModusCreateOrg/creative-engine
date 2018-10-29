#include "GGame.h"

GGame::GGame() {
  gResourceManager.LoadBitmap(CHARSET_16X16_BMP, FONT_16x16_SLOT, IMAGE_16x16);
  gResourceManager.LoadBitmap(CHARSET_8X8_BMP, FONT_8x8_SLOT, IMAGE_8x8);

  gViewPort = new BViewPort();
  gViewPort->Offset(25, 50);
  gViewPort->SetRect(TRect(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1));

  mState = mNextState = -1;
  gGameEngine = ENull;
  SetState(GAME_STATE_SPLASH);
}

GGame::~GGame() {
  delete gGameEngine;
  delete gViewPort;
  gResourceManager.ReleaseBitmapSlot(FONT_8x8_SLOT);
  gResourceManager.ReleaseBitmapSlot(FONT_16x16_SLOT);
}

void GGame::SetState(TInt aNewState) {
  mNextState = aNewState;
}

void GGame::Run() {
  while (1) {
    Random(); // randomize

    if (mNextState != mState) {
      switch (mNextState) {
        case GAME_STATE_SPLASH:
          delete gGameEngine;
          gGameEngine = new GSplashState();
          break;
        case GAME_STATE_GAME:
          delete gGameEngine;
          gGameEngine = new GGameState();
          break;
        default:
          continue;
      }

      // reset dKeys so next state doesn't react to any keys already pressed
      gControls.dKeys = 0;
      mState = mNextState;
    }

    gGameEngine->GameLoop();
    gDisplay.Update();

    if (gControls.WasPressed(BUTTONQ)) {
      break;
    }
  }
}
