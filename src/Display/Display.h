#ifndef DISPLAY_H
#define DISPLAY_H

#include "BBase.h"
#include "BBitmap.h"
#include "DisplayDefines.h"
#include <unistd.h>
#include "SDL_timer.h"

const TInt FRAMES_PER_SECOND = 60;

class Display {
public:
  Display() {
    mBitmap1      = BBitmap::CreateBBitmap(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, MEMF_FAST);
    mBitmap2      = BBitmap::CreateBBitmap(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, MEMF_FAST);
    renderBitmap  = mBitmap1;
    displayBitmap = mBitmap2;

    mRate = 1000 / FRAMERATE;
    mElapsed = 0;
    mSLastTicks = SDL_GetTicks();
  }

  ~Display() {
    delete mBitmap1;
    delete mBitmap2;
  };

  virtual void Update() = 0;

  virtual void Init() = 0;

  void SetPalette(TRGB aPalette[], TInt aIndex=0, TInt aCount = 256) {
    displayBitmap->SetPalette(aPalette, aIndex, aCount);
    renderBitmap->SetPalette(aPalette, aIndex, aCount);
  }

  void SetPalette(BBitmap *aBitmap, TInt aIndex=0, TInt aCount = 256) {
    displayBitmap->SetPalette(aBitmap, aIndex, aCount);
    renderBitmap->SetPalette(aBitmap, aIndex, aCount);
  }

  void SetColor(TUint8 aIndex, TUint8 aRed, TUint8 aGreen, TUint8 aBlue) {
    displayBitmap->SetColor(aIndex, aRed, aGreen, aBlue);
    renderBitmap->SetColor(aIndex, aRed, aGreen, aBlue);
  }
  void SetColor(TUint8 aIndex, TRGB& aColor) {
    displayBitmap->SetColor(aIndex, aColor.r, aColor.g, aColor.b);
    renderBitmap->SetColor(aIndex, aColor.r, aColor.g, aColor.b);
  }


  virtual TUint16 color565(TUint8 red, TUint8 green, TUint8 blue) = 0;


  void NextFrameDelay() {
#ifndef __DINGUX__
    Uint32 currentTicks = SDL_GetTicks();
    mElapsed = currentTicks - mSLastTicks;

    auto delay = (TUint32)ceil(mRate - mElapsed);

    if (mElapsed < mRate) {
      SDL_Delay(delay);
    }

    currentTicks = SDL_GetTicks();
    mElapsed = (TUint32)fmin(currentTicks - mSLastTicks, 10000);
    mSLastTicks = SDL_GetTicks();
#endif
  }


protected:
  // 2 bitmaps for double buffering
  BBitmap *mBitmap1, *mBitmap2;

  void SwapBuffers() {
    if (renderBitmap == mBitmap1) {
      renderBitmap = mBitmap2;
      displayBitmap = mBitmap1;
    }
    else {
      renderBitmap = mBitmap1;
      displayBitmap = mBitmap2;
    }
  }
public:
  BBitmap *displayBitmap, *renderBitmap;

  const TUint32 FRAMERATE = FRAMES_PER_SECOND;
  TUint32       mSLastTicks, mElapsed, mRate;
};

extern Display &gDisplay;
extern TRect gScreenRect;

#endif //DISPLAY_H
