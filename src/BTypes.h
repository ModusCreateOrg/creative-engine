#ifndef BTYPES_H
#define BTYPES_H

#include <stddef.h>
#include <stdio.h>

typedef unsigned char  TUint8;
typedef unsigned short TUint16;
typedef unsigned int   TUint32;
typedef unsigned int   TUint;
typedef unsigned long  TUint64;

typedef char  TInt8;
typedef short TInt16;
typedef int   TInt32;
typedef long  TInt64;

typedef int  TInt;
typedef void TAny;
typedef bool TBool;

typedef float TFloat;

#define ETrue (true)
#define EFalse (false)

#define ENull NULL

#ifndef LOBYTEM
#define LOBYTE(x) TUint8((x)&0xff)
#endif

#ifndef HIBYTE
#define HIBYTE(x) TUint8(((x) >> 8) & 0xff)
#endif

#ifndef LOWORD
#define LOWORD(x) TUint16((x)&0xffff)
#endif

#ifndef HIWORD
#define HIWORD(x) TUint16(((x) >> 16) & 0xffff)
#endif

#ifndef ABS
#define ABS(x) ((x) < 0 ? -(x) : (x))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? a : b)
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? a : b)
#endif

#ifndef MID
#define MID(a, b) (((a) + (b)) / 2)
#endif

#define MAXINT (0x7fffffff)

#define ROUND_UP_4(x) (((x) + 3) & -4)

struct TRGB {
  TUint8 r, g, b;

public:
  TRGB() {
    r = g = b = 0;
  }

  TRGB(TUint32 aColor) {
    r = TUint8((aColor >> 16) & 0xff);
    g = TUint8((aColor >> 8) & 0xff);
    b = TUint8((aColor >> 0) & 0xff);
  }

  TRGB(TUint8 aRed, TUint8 aGreen, TUint8 aBlue) {
    r = aRed;
    g = aGreen;
    b = aBlue;
  }

  TRGB(TRGB &aOther) {
    r = aOther.r;
    g = aOther.g;
    b = aOther.b;
  }

public:
  void Set(TUint8 aRed, TUint8 aGreen, TUint8 aBlue) {
    r = aRed;
    g = aGreen;
    b = aBlue;
  }

  void Set(TRGB &aOther) {
    r = aOther.r;
    g = aOther.g;
    b = aOther.b;
  }

  void Scale(TUint aScale) {
    r = (r * aScale) >> 8;
    g = (g * aScale) >> 8;
    b = (b * aScale) >> 8;
  }

  void SetScaled(TUint8 aRed, TUint8 aGreen, TUint8 aBlue, TUint aScale) {
    r = (aRed * aScale) >> 8;
    g = (aGreen * aScale) >> 8;
    b = (aBlue * aScale) >> 8;
  }

public:
  TUint32 rgb888() { return (r << 16) | (g << 8) | (b << 0); }

  TUint16 rgb565() {
    return ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
  }

  TUint16 bgr565() {
    return ((b & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (r >> 3);
  }

  TUint16 brg565() {
    return ((b & 0b11111000) << 8) | ((r & 0b11111100) << 3) | (g >> 3);
  }

public:
  void Dump() {
    printf("%02x:%02x:%02x\n", r, g, b);
  }
};

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
    this->x1 = 0.0;
    this->y1 = 0.0;
    this->x2 = 0.0;
    this->y2 = 0.0;
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

#endif // BTYPES_H
