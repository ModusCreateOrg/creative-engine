#ifndef GENUS_TRECT_H
#define GENUS_TRECT_H

#include "BTypes.h"

struct TPoint {
  TInt32 x, y;

  TPoint() {
    x = 0.0;
    y = 0.0;
  }

  TPoint(TInt32 aX, TInt32 aY) {
    x = aX;
    y = aY;
  }

  TPoint(const TPoint &aOther) {
    x = aOther.x;
    y = aOther.y;
  }

  void Set(TInt32 aX, TInt32 aY) {
    x = aX;
    y = aY;
  }

  void Set(const TPoint &aOther) {
    x = aOther.x;
    y = aOther.y;
  }

  void Offset(TInt32 aX, TInt32 aY) {
    x += aX;
    y += aY;
  }

  void Offset(const TPoint &aOther) {
    x += aOther.x;
    y += aOther.y;
  }
};

struct TRect {
  TInt32 x1, y1, x2, y2;

  // Constructors
public:
  TRect() {
    this->x1 = 0;
    this->y1 = 0;
    this->x2 = 0;
    this->y2 = 0;
  }

  TRect(TInt32 aX1, TInt32 aY1, TInt32 aX2, TInt32 aY2) {
    this->x1 = aX1;
    this->y1 = aY1;
    this->x2 = aX2;
    this->y2 = aY2;
  }

  TRect(const TPoint &aUpperLeft, const TPoint &aLowerRight) {
    this->x1 = aUpperLeft.x;
    this->y1 = aUpperLeft.y;
    this->x2 = aLowerRight.x;
    this->y2 = aLowerRight.y;
  }

  TRect(const TRect &aOther) {
    this->x1 = aOther.x1;
    this->y1 = aOther.y1;
    this->x2 = aOther.x2;
    this->y2 = aOther.y2;
  }

  void Set(TInt32 aX1, TInt32 aY1, TInt32 aX2, TInt32 aY2) {
    this->x1 = aX1;
    this->y1 = aY1;
    this->x2 = aX2;
    this->y2 = aY2;
  }

  void Set(const TPoint &aUpperLeft, const TPoint &aLowerRight) {
    this->x1 = aUpperLeft.x;
    this->y1 = aUpperLeft.y;
    this->x2 = aLowerRight.x;
    this->y2 = aLowerRight.y;
  }

  void Set(const TRect &aOther) {
    this->x1 = aOther.x1;
    this->y1 = aOther.y1;
    this->x2 = aOther.x2;
    this->y2 = aOther.y2;
  }

public:
  TInt32 Width() { return (x2 - x1) + 1; }

  TInt32 Height() { return (y2 - y1) + 1; }

  void Width(TInt32 aWidth) { x2 = x1 + aWidth - 1; }

  void Height(TInt32 aHeight) { y2 = y1 + aHeight - 1; }

public:
  // offset (move) by detla in resources and y
  void Offset(TInt aDX, TInt aDY) {
    x1 += aDX;
    y1 += aDY;
    x2 += aDX;
    y2 += aDY;
  }

public:
  TBool Overlaps(TInt32 aX1, TInt32 aY1, TInt32 aX2, TInt32 aY2);

  TBool Overlaps(const TPoint &aUpperLeft, const TPoint);

  TBool Overlaps(TRect &aOther);

public:
  void Normalize();

  void Union(const TRect &aRect);

  TBool Intersection(const TRect &aSrcRect);

  TBool Intersection(const TRect &aSrcRect1, const TRect &aSrcRect2);

public:
  TBool PointInRect(TInt x, TInt y) {
    return x >= x1 && x <= x2 && y >= y1 && y <= y2;
  }

  TBool PointInRect(TPoint p) { return PointInRect(p.x, p.y); }

public:
  void Center(TInt32 aX, TInt32 aY);

  void Center(TPoint &aPoint) { Center(aPoint.x, aPoint.y); }

  void Center(TRect &aRect) {
    Center(MID(aRect.x1, aRect.x2), MID(aRect.y1, aRect.y2));
  }

  void CenterX(TInt32 aX);

  void CenterX(TRect &aRect) { CenterX(MID(aRect.x1, aRect.x2)); }

  void CenterY(TInt32 aY);

  void CenterY(TRect &aRect) { CenterY(MID(aRect.y1, aRect.y2)); }

  void CenterInX(TRect &aRect); // centers within rect (typically ViewPort rect)
  void CenterInY(TRect &aRect); // centers within rect (typically ViewPort rect)
  void CenterIn(TRect &aRect);  // centers within rect (typically ViewPort rect)

public:
};

#endif //GENUS_TRECT_H
