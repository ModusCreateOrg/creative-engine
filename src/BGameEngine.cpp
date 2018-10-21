#include "BGameEngine.h"
#include "Display.h"
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

/**
 * Resets the engine
 *
 *
 */
void BGameEngine::Reset() {
  gSpriteList.Reset();
  gProcessList.Genocide();
}

void BGameEngine::AddProcess(BProcess *aProcess) {
  gProcessList.AddProcess(aProcess);
}

void BGameEngine::RunProcessesBefore() {
  gProcessList.RunBefore();
}

void BGameEngine::RunProcessesAfter() {
  gProcessList.RunAfter();
}

void BGameEngine::GameLoop() {
  gControls.Poll();
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
      gSpriteList.Move();
      gSpriteList.Animate();
      gProcessList.RunBefore();
    }
    gSpriteList.Render(mViewPort);

    if (!mPauseFlag) {
      gProcessList.RunAfter();
    }
  }

  PostRender();
}