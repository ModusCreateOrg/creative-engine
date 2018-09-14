//
// Created by Michael Schwartz on 9/5/18.
//

#include "BGameEngine.h"
#include "Display.h"
#include "Controls.h"

BGameEngine::BGameEngine(BViewPort *aViewPort) {
  mWorldXX = mWorldYY = 0;
  mPlayfield = ENull;
  mPauseFlag = EFalse;
  mEnabled = ETrue;
  mFrameCounter = 0;
  mDisplay = &display;
  mViewPort = aViewPort;
}

BGameEngine::~BGameEngine() {
  delete mPlayfield;
  mPlayfield = ENull;
}

// you can set viewport here for game area
void BGameEngine::PreRender() {
}

// and use score/status viewport here
// typically games will use this to draw the score/status area, too
void BGameEngine::PostRender() {
}

void BGameEngine::GameLoop() {
  controls.Poll();
  mFrameCounter++;
  PreRender();

  if (mEnabled) {
    if (mPlayfield) {
      // allow inheritor to set mWorldXX,mWorldYY as desired
      PositionCamera();
      // animate the playfield
      mPlayfield->Animate();

      mPlayfield->Render();
    }
    if (!mPauseFlag) {
      spriteList.Move();
      spriteList.Animate();
      processList.RunBefore();
    }
    spriteList.Render(mViewPort);

    if (!mPauseFlag) processList.RunAfter();
  }

  PostRender();
}