#include "BGameEngine.h"
#include "Display/Display.h"
#include "Controls.h"

BGameEngine::BGameEngine(BViewPort *aViewPort) {
  mWorldXX = mWorldYY = 0;
  mPlayfield = ENull;
  mPauseFlag = EFalse;
  mEnabled = ETrue;
  mFrameCounter = 0;
  mDisplay = &gDisplay;
  mViewPort = aViewPort;
}

BGameEngine::~BGameEngine() {
  // assure processes and sprites get freed
  Reset();
  if (mPlayfield) {
    delete mPlayfield;
    mPlayfield = ENull;
  }
}

// you can set viewport here for game area
void BGameEngine::PreRender() {
}

// and use score/status viewport here
// typically games will use this to draw the score/status area, too
void BGameEngine::PostRender() {
}

/**
 * Resets the engine
 *
 *
 */
void BGameEngine::Reset() {
  mProcessList.Genocide();
  mSpriteList.Reset();
}

void BGameEngine::AddProcess(BProcess *aProcess) {
  mProcessList.AddProcess(aProcess);
}

void BGameEngine::RunProcessesBefore() {
  mProcessList.RunBefore();
}

void BGameEngine::RunProcessesAfter() {
  mProcessList.RunAfter();
}

void BGameEngine::GameLoop() {
  if (!mPauseFlag) {
    gControls.Poll();
  }

  mFrameCounter++;
  PreRender();

  if (mEnabled) {
    if (mPlayfield) {
      // allow inheritor to set mWorldXX,mWorldYY as desired
      PositionCamera();
      // animate the playfield
      mPlayfield->Animate();

      if (mPlayfield != ENull) {
        mPlayfield->Render();
      }
    }

    if (!mPauseFlag) {
      mSpriteList.Move();
      mSpriteList.Animate();
      mProcessList.RunBefore();
    }
    mSpriteList.Render(mViewPort);

    if (!mPauseFlag) {
      mProcessList.RunAfter();
    }
  }

  PostRender();
}
