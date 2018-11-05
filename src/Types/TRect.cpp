#include "TRect.h"

TBool TRect::Overlaps(TRect &aOther) {
  if (x1 > aOther.x2 || x2 < aOther.x1) {
    return EFalse;
  }
  if (y1 > aOther.y2 || y2 < aOther.y1) {
    return EFalse;
  }
  return ETrue;
}

void TRect::Normalize() {
  TRect t = *this;
  x1 = MIN(t.x1, t.x2);
  x2 = MAX(t.x1, t.x2);
  y1 = MIN(t.y1, t.y2);
  y2 = MAX(t.y1, t.y2);
}

// enlarge this rect to encompass given rect
void TRect::Union(const TRect &aRect) {
  x1 = MIN(x1, aRect.x1);
  y1 = MIN(y1, aRect.y1);
  x2 = MAX(x2, aRect.x2);
  y2 = MAX(y2, aRect.y2);
}

// set this rect to the overlapping region between given src rects
TBool TRect::Intersection(const TRect &aSrcRect1, const TRect &aSrcRect2) {
  *this = aSrcRect1;
  return Intersection(aSrcRect2);
}

// set this rect to the overlapping region between this rect and given src rect
TBool TRect::Intersection(const TRect &aSrcRect) {
  x1 = MAX(x1, aSrcRect.x1);
  x2 = MIN(x2, aSrcRect.x2);
  y1 = MAX(y1, aSrcRect.y1);
  y2 = MIN(y2, aSrcRect.y2);
  if (x2 < x1 || y2 < y1) {
    x1 = x2 = y1 = y2 = -1; // no overlap^M
    return EFalse;
  }
  Normalize();
  return ETrue;
}

// move this rect in X centered around given X
void TRect::CenterX(TInt32 aX) {
  TInt32 dx = aX - MID(x1, x2);
  x1 += dx;
  x2 += dx;
}

// move this rect in Y centered around given Y
void TRect::CenterY(TInt32 aY) {
  TInt32 dy = aY - MID(y1, y2);
  y1 += dy;
  y2 += dy;
}

// move this rect in X and Y centered around given X,Y
void TRect::Center(TInt32 aX, TInt32 aY) {
  TInt32 dx = aX - MID(x1, x2);
  x1 += dx;
  x2 += dx;

  TInt32 dy = aY - MID(y1, y2);
  y1 += dy;
  y2 += dy;
}

// move this rect in X centered around given Rect
void TRect::CenterInX(TRect &aRect) {
  CenterX(MID(aRect.x1, aRect.x2));
  x1 -= aRect.x1;
  x2 -= aRect.x1;
}

// move this rect in Y centered around given Rect
void TRect::CenterInY(TRect &aRect) {
  CenterY(MID(aRect.y1, aRect.y2));
  y1 -= aRect.y1;
  y2 -= aRect.y1;
}

// move this rect in X and Y centered around given Rect
void TRect::CenterIn(TRect &aRect) {
  Center(MID(aRect.x1, aRect.x2), MID(aRect.y1, aRect.y2));
  x1 -= aRect.x1;
  x2 -= aRect.x1;
  y1 -= aRect.y1;
  y2 -= aRect.y1;
}
