#ifndef BVIEWPORT_H
#define BVIEWPORT_H

#include "BBase.h"

/**
 * BViewPort encapsulates the view port or displayable area of the world on screen.  For games that
 * scroll horizontally, the BViewPort's x will be incremented and the things in the world will appear
 * to move to the left.  The things in the world are simply rendered relative to the BViewPort.  All
 * things and the BViewPort have world coordinates.
 */
class BViewPort : public BBase {
public:
  BViewPort();
  ~BViewPort();
public:
  void Offset(TInt aDx, TInt aDy) {
    mOffsetX = aDx;
    mOffsetY = aDy;
  }
  void SetRect(TRect aRect) {
    mRect.Set(aRect);
  }
  TRect GetRect() { return mRect; }
  void GetRect(TRect& aRect) { aRect.Set(mRect); }
public:
  void Dump();
public:
  TFloat mWorldX, mWorldY;
  TRect mRect;
  // offset to upper left corner of viewport on screen
 TFloat mOffsetX, mOffsetY;
};

#endif
