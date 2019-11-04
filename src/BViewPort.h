#ifndef BVIEWPORT_H
#define BVIEWPORT_H

#include "BBase.h"
#include "BBitmap.h"

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

  void DrawFastHLine(BBitmap *aBitmap, TInt aX, TInt aY, TUint aW, TUint8 aColor) {
    aBitmap->DrawFastHLine(this, aX, aY, aW, aColor);
  }
  void DrawFastHLine(BBitmap *aBitmap, TInt aX, TInt aY, TUint aW, TUint32 aColor) {
    aBitmap->DrawFastHLine(this, aX, aY, aW, aColor);
  }
  void DrawFastHLine(BBitmap *aBitmap, TInt aX, TInt aY, TUint aW, const TRGB &aColor) {
    aBitmap->DrawFastHLine(this, aX, aY, aW, aColor.rgb888());
  }

  void DrawFastVLine(BBitmap *aBitmap, TInt aX, TInt aY, TUint aH, TUint8 aColor) {
    aBitmap->DrawFastHLine(this, aX, aY, aH, aColor);
  }
  void DrawFastVLine(BBitmap *aBitmap, TInt aX, TInt aY, TUint aH, TUint32 aColor) {
    aBitmap->DrawFastHLine(this, aX, aY, aH, aColor);
  }
  void DrawFastVLine(BBitmap *aBitmap, TInt aX, TInt aY, TUint aH, const TRGB &aColor) {
    aBitmap->DrawFastHLine(this, aX, aY, aH, aColor);
  }

  void DrawLine(BBitmap *aBitmap, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint8 aColor) {
    aBitmap->DrawLine(this, aX1, aY1, aX2, aY2, aColor);
  }
  void DrawLine(BBitmap *aBitmap, TRect &aRect, TUint8 aColor) {
    aBitmap->DrawLine(this, aRect, aColor);
  }

  void DrawLine(BBitmap *aBitmap, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint32 aColor) {
    aBitmap->DrawLine(this, aX1, aY1, aX2, aY2, aColor);
  }
  void DrawLine(BBitmap *aBitmap, TRect &aRect, TUint32 aColor) {
    aBitmap->DrawLine(this, aRect, aColor);
  }

  void DrawLine(BBitmap *aBitmap, TInt aX1, TInt aY1, TInt aX2, TInt aY2, const TRGB &aColor) {
    aBitmap->DrawLine(this, aX1, aY1, aX2, aY2, aColor);
  }
  void DrawLine(BBitmap *aBitmap, TRect &aRect, const TRGB &aColor) {
    aBitmap->DrawLine(this, aRect, aColor);
  }

  void DrawRect(BBitmap *aBitmap, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint8 aColor) {
    aBitmap->DrawRect(this, aX1, aY1, aX2, aY2, aColor);
  }
  void DrawRect(BBitmap *aBitmap, TRect &aRect, TUint8 aColor) {
    aBitmap->DrawRect(this, aRect, aColor);
  }

  void DrawRect(BBitmap *aBitmap, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint32 aColor) {
    aBitmap->DrawRect(this, aX1, aY1, aX2, aY2, aColor);
  }
  void DrawRect(BBitmap *aBitmap, TRect &aRect, TUint32 aColor) {
    aBitmap->DrawRect(this, aRect, aColor);
  }

  void DrawRect(BBitmap *aBitmap, TInt aX1, TInt aY1, TInt aX2, TInt aY2, const TRGB &aColor) {
    aBitmap->DrawRect(this, aX1, aY1, aX2, aY2, aColor);
  }
  void DrawRect(BBitmap *aBitmap, TRect &aRect, const TRGB &aColor) {
    aBitmap->DrawRect(this, aRect, aColor);
  }

  void FillRect(BBitmap *aBitmap, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint8 aColor) {
    aBitmap->FillRect(this, aX1, aY1, aX2, aY2, aColor);
  }
  void FillRect(BBitmap *aBitmap, TRect &aRect, TUint8 aColor) {
    aBitmap->FillRect(this, aRect, aColor);
  }

  void FillRect(BBitmap *aBitmap, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint32 aColor) {
    aBitmap->FillRect(this, aX1, aY1, aX2, aY2, aColor);
  }
  void FillRect(BBitmap *aBitmap, TRect &aRect, TUint32 aColor) {
    aBitmap->FillRect(this, aRect, aColor);
  }

  void FillRect(BBitmap *aBitmap, TInt aX1, TInt aY1, TInt aX2, TInt aY2, const TRGB &aColor) {
    aBitmap->FillRect(this, aX1, aY1, aX2, aY2, aColor);
  }
  void FillRect(BBitmap *aBitmap, TRect &aRect, const TRGB &aColor) {
    aBitmap->FillRect(this, aRect, aColor);
  }

  void DrawCircle(BBitmap *aBitmap, TInt aX, TInt aY, TUint r, TUint8 aColor) {
    aBitmap->DrawCircle(this, aX, aY, r, aColor);
  }
  void DrawCircle(BBitmap *aBitmap, TInt aX, TInt aY, TUint r, TUint32 aColor) {
    aBitmap->DrawCircle(this, aX, aY, r, aColor);
  }
  void DrawCircle(BBitmap *aBitmap, TInt aX, TInt aY, TUint r, const TRGB &aColor) {
    aBitmap->DrawCircle(this, aX, aY, r, aColor);
  }

  void FillCircle(BBitmap *aBitmap, TInt aX, TInt aY, TUint r, TUint8 aColor) {
    aBitmap->FillCircle(this, aX, aY, r, aColor);
  }
  void FillCircle(BBitmap *aBitmap, TInt aX, TInt aY, TUint r, TUint32 aColor) {
    aBitmap->FillCircle(this, aX, aY, r, aColor);
  }
  void FillCircle(BBitmap *aBitmap, TInt aX, TInt aY, TUint r, const TRGB &aColor) {
    aBitmap->FillCircle(this, aX, aY, r, aColor);
  }

public:
  /**
   * Draw a rectangle from src bitmap to destination bitmap.
   *
   * @param aBitmap
   * @param aSrcBitmap
   * @param aSrcRect
   * @param aDstX
   * @param aDstY
   * @param aFlags
   * @return
   */
  TBool DrawBitmap(BBitmap *aBitmap, BBitmap *aSrcBitmap, TRect aSrcRect,
                   TInt aDstX, TInt aDstY, TUint32 aFlags = 0) {
    aBitmap->DrawBitmap(this, aSrcBitmap, aSrcRect, aDstX, aDstY, aFlags);
  }

  /**
   * Draw a rectangle from src bitmap to destination bitmap.
   * Flip/flop/etc. are indicated by the BSprite flags bits passed in.
   * Transparency is considered (e.g. transparent pixels not rendered).
   *
   * @param aBitmap
   * @param aSrcBitmap
   * @param aSrcRect
   * @param aDstX
   * @param aDstY
   * @param aFlags
   * @return
   */
  TBool DrawBitmapTransparent(BBitmap *aBitmap, BBitmap *aSrcBitmap,
                              TRect aSrcRect, TInt aDstX, TInt aDstY, TUint32 aFlags = 0) {
    aBitmap->DrawBitmapTransparent(this, aSrcBitmap, aSrcRect, aDstX, aDstY, aFlags);
  }

  /**
   * Draw a text string using a specific font
   * Transparent pixels are considered if aBgColor is -1
   *
   * @param aBitmap
   * @param aStr
   * @param aFont
   * @param aDstX
   * @param aDstY
   * @param aFgColor
   * @param aBgColor
   * @param aLetterSpacing
   * @return
   */
  TBool DrawString(BBitmap *aBitmap, const char *aStr, const BFont *aFont,
                   TInt aDstX, TInt aDstY, TUint8 aFgColor, TInt16 aBgColor = -1,
                   TInt aLetterSpacing = 0) {
    aBitmap->DrawString(this, aStr, aFont, aDstX, aDstY, aFgColor, aBgColor, aLetterSpacing);
  }
  TBool DrawString(BBitmap *aBitmap, const char *aStr, const BFont *aFont,
                   TInt aDstX, TInt aDstY, TUint32 aFgColor, TInt32 aBgColor = -1,
                   TInt aLetterSpacing = 0) {
    aBitmap->DrawString(this, aStr, aFont, aDstX, aDstY, aFgColor, aBgColor, aLetterSpacing);
  }
  TBool DrawString(BBitmap *aBitmap, const char *aStr, const BFont *aFont,
                   TInt aDstX, TInt aDstY, const TRGB &aFgColor, const TRGB &aBgColor,
                   TInt aLetterSpacing = 0) {
    aBitmap->DrawString(this, aStr, aFont, aDstX, aDstY, aFgColor, aBgColor, aLetterSpacing);
  }

  TBool DrawStringShadow(BBitmap *aBitmap, const char *aStr,
                         const BFont *aFont, TInt aDstX, TInt aDstY, TUint8 aFgColor,
                         TUint8 aShadowColor, TInt16 aBgColor = -1, TInt aLetterSpacing = 0) {
    aBitmap->DrawStringShadow(this, aStr, aFont, aDstX, aDstY, aFgColor, aShadowColor, aBgColor, aLetterSpacing);
  }
  TBool DrawStringShadow(BBitmap *aBitmap, const char *aStr,
                         const BFont *aFont, TInt aDstX, TInt aDstY, TUint32 aFgColor,
                         TUint32 aShadowColor, TInt32 aBgColor = -1, TInt aLetterSpacing = 0) {
    aBitmap->DrawStringShadow(this, aStr, aFont, aDstX, aDstY, aFgColor, aShadowColor, aBgColor, aLetterSpacing);
  }
  TBool DrawStringShadow(BBitmap *aBitmap, const char *aStr,
                         const BFont *aFont, TInt aDstX, TInt aDstY, const TRGB &aFgColor,
                         const TRGB &aShadowColor, const TRGB &aBgColor, TInt aLetterSpacing = 0) {
    aBitmap->DrawStringShadow(this, aStr, aFont, aDstX, aDstY, aFgColor, aShadowColor, aBgColor, aLetterSpacing);
  }

};

#endif
