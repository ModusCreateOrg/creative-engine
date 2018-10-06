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
  mSpriteList = new BSpriteList();
  mProcessList = new BProcessList();
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

/**
 * Resets the engine
 *
 *
 */
void BGameEngine::Reset() {
  mSpriteList->Reset();
  mProcessList->Reset();
}

void BGameEngine::AddProcess(BProcess *aProcess) {
  mProcessList->AddProcess(aProcess);
}

void BGameEngine::RunProcessesBefore() {
  mProcessList->RunBefore();
}

void BGameEngine::RunProcessesAfter() {
  mProcessList->RunAfter();
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
      mSpriteList->Move();
      mSpriteList->Animate();
      mProcessList->RunBefore();
    }
    mSpriteList->Render(mViewPort);

    if (!mPauseFlag) {
      mProcessList->RunAfter();
    }
  }

  PostRender();
}