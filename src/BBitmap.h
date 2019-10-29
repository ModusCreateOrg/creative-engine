#ifndef BBITMAP_H
#define BBITMAP_H

#include "BBase.h"

const TUint32 DRAW_NORMAL = 0;
const TUint32 DRAW_FLIPPED = (1 << 0);
const TUint32 DRAW_FLOPPED = (1 << 1);
const TUint32 DRAW_ROTATE_RIGHT = (1 << 2);
const TUint32 DRAW_ROTATE_LEFT = (1 << 3);

class BResourceManager;

class Display;

class BViewPort;

class BFont;

/**
 * BBitmap * holds the common kinds of properties of bitmaps: width,
 * height, bits per pixel, Palette, and Pixel data, etc.
 *
 * Bitmaps have several attributes:
 * 1) Bits Per Pixel (BPP)
 * 2) Palette (based upon BPP)
 * 3) RAM vs. ROM (ROM is immutable)
 * 4) Optimal read/write pixel routines based upon BPP
 *
 * For the permutations of these attributes that make sense, we implement
 * "interfaces" BRAMBitmap, BROMBitmap, which provide the functionality for
 * writing and reading pixels, respectively.  It doesn't make sense to provide
 * write methods for bitmaps whose pixels are in FLASH/ROM, though both ROM and
 * RAM based bitmaps should be readable (ReadPixel()).
 *
 * The BBitmapBase class holds the common kinds of properties of bitmaps: width,
 * height, bits per pixel, Palette, and Pixel data, etc.
 *
 * ReadPixel()/WritePixel() are typically called within nested (for y, then for
 * x) loops. Adding any bounds checking or bit depth checking in these routines
 * adds considerable CPU cycles to almost all rendering functions.  So we
 * implement Bitmap classes for each of 8 BPP, 4BPP, and 1 BPP, each with their
 * own optimized ReadPixel()/WritePixel() routines.  We do provide a
 * SafeWritePixel() method that does bounds checking, but care should be used
 * when writing code that calls it, to assure we're not adding unnecessary CPU
 * cycles to the rendering function.
 *
 * There is no class constructor that we would call to instantiate bitmaps.
 * Instead, we call a factory method in BResourceManager that returns a somewhat
 * opaque handle to the constructed bitmap.  What makes it somewhat opaque is
 * the bit depth is inherent in the form of the class instance returned.
 *
 * There are factory methods in BResourceManager to create an instance of a RAM
 * bitmap or an instance of a ROM/FLASH one.  For ROM bitmaps, the Palette does
 * exist in RAM.  Potentially, we might want to render the same bitmap using
 * differing palettes.
 *
 * The structure, as implemented, is designed to facilitate rendering different
 * BPP bitmaps.  In other words, you can render a 1 BPP bitmap in an 8 BPP one.
 * A common use case is to have 16 color sprites that you want to draw onto the
 * 8 BPP screen buffer.
 *
 */
class BBitmap : public BBase {
  /**
    * public factory methods
    */
public:
  static BBitmap *CreateBBitmap(TUint aWidth, TUint aHeight, TUint aDepth, TUint16 aMemoryFlag = MEMF_SLOW);

  static BBitmap *CreateBBitmap(TAny *aRom, TUint16 aMemoryFlag = MEMF_SLOW);

private:
  /*
   * Constructors are PRIVATE - use the factory methods!
   */
  BBitmap(TUint aWidth, TUint aHeight, TUint aDepth, TUint16 aMemoryFlag = MEMF_SLOW);

  BBitmap(TAny *aRom, TUint16 aMemoryFlag = MEMF_SLOW);

public:
  virtual ~BBitmap();

  TUint mPitch;

public:
  TUint Width() { return mWidth; }

  TUint Height() { return mHeight; }

  TUint Depth() { return mDepth; }

  TInt TransparentColor() { return mTransparentColor; }

  TRect &Dimensions() { return mDimensions; }

  TUint8 *GetPixels() { return mPixels; }

  /**
   * Remap bitmap pixels and palette so it fits into aOther.
   *
   * aOther's palette is modified to add any new colors needed.
   */
  void Remap(BBitmap *aOther);

public:
  void Dump();

public:
  /**
   * Count the colors used in the Bitmap
   * @return
   */
  TInt CountColors();

  TInt CountUsedColors();

  /**
   * Return index in palette of next unused color, or -1 if none
   * @return
   */
  TInt NextUnusedColor();

  /**
   * Find a used color in the palette.
   *
   * Return index in palette of found color, or -1 if not found.
   */
  TInt FindColor(const TRGB &aColor);

  TInt FindColor(TInt aRed, TInt aGreen, TInt aBlue) {
    return FindColor(TRGB(aRed, aGreen, aBlue));
  }

  /**
   * Mark a color in palette as used.
   */
  void UseColor(TInt index) { mColorsUsed[index] = ETrue; }

  /**
   * Set the BBitmap's palette from an array of TRGB
   *
   * @param aPalette
   * @param aCount
   */
  void SetPalette(TRGB aPalette[], TInt aIndex = 0, TInt aCount = 256);

  /**
   * Set palette by copying palette from another BBitmap
   * @param aBitmap
   * @param aCount
   */
  void SetPalette(BBitmap *aBitmap, TInt aIndex = 0, TInt aCount = 256) {
    SetPalette(aBitmap->mPalette, aIndex, aCount);
  }

  void SetColor(TUint8 index, TUint8 r, TUint8 g, TUint8 b) {
    mPalette[index].Set(r, g, b);
  }

  void SetColor(TUint8 index, const TRGB &aColor) {
    mPalette[index].Set(aColor.r, aColor.g, aColor.b);
  }

  TRGB &GetColor(TUint8 index) { return mPalette[index]; }

  static void CheckScreenDepth();

public:
  TRGB &ReadColor(TInt aX, TInt aY) {
    TUint8 pixel = mPixels[aY * mPitch + aX];
    return mPalette[pixel];
  }

  TUint32 ReadPixelColor(TInt aX, TInt aY) { return ReadColor(aX, aY).rgb888(); }

  TUint8 ReadPixel(TInt aX, TInt aY) { return mPixels[aY * mPitch + aX]; }

  void WritePixel(TInt aX, TInt aY, TUint8 aColor);
  void WritePixel(TInt aX, TInt aY, const TRGB &aColor);

  void SafeWritePixel(TInt aX, TInt aY, TUint8 aColor);
  void SafeWritePixel(TInt aX, TInt aY, const TRGB &aColor);

public:
  // DRAWING PRIMITIVES
  /**
   * Erase bitmap to specified color
   */
  void Clear(const TRGB &aColor);
  void Clear(TUint8 aIndex = 0);

  /**
   * Copy the pixels from one bitmap to another.
   *
   * @param aOther
   */
  void CopyPixels(BBitmap *aOther);

  void DrawFastHLine(BViewPort *aViewPort, TInt aX, TInt aY, TUint aW, TUint8 aColor);

  void DrawFastVLine(BViewPort *aViewPort, TInt aX, TInt aY, TUint aH, TUint8 aColor);

  void DrawLine(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint8 aColor);
  void DrawLine(BViewPort *aViewPort, TRect &aRect, TUint8 aColor) {
    DrawLine(aViewPort, aRect.x1, aRect.y1, aRect.x2, aRect.y2, aColor);
  }

  void DrawRect(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint8 aColor);
  void DrawRect(BViewPort *aViewPort, TRect &aRect, TUint8 aColor) {
    DrawRect(aViewPort, aRect.x1, aRect.y1, aRect.x2, aRect.y2, aColor);
  }

  void DrawRect(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint32 aColor);
  void DrawRect(BViewPort *aViewPort, TRect &aRect, TUint32 aColor) {
    DrawRect(aViewPort, aRect.x1, aRect.y1, aRect.x2, aRect.y2, aColor);
  }

  void FillRect(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint8 aColor);
  void FillRect(BViewPort *aViewPort, TRect &aRect, TUint8 aColor) {
    FillRect(aViewPort, aRect.x1, aRect.y1, aRect.x2, aRect.y2, aColor);
  }

  void FillRect(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint32 aColor);
  void FillRect(BViewPort *aViewPort, TRect &aRect, TUint32 aColor) {
    FillRect(aViewPort, aRect.x1, aRect.y1, aRect.x2, aRect.y2, aColor);
  }

  void DrawCircle(BViewPort *aViewPort, TInt aX, TInt aY, TUint r, TUint8 aColor);

  void FillCircle(BViewPort *aViewPort, TInt aX, TInt aY, TUint r, TUint8 aColor);

public:
  /**
   * Draw a rectangle from src bitmap to destination bitmap.
   *
   * @param aViewPort
   * @param aSrcBitmap
   * @param aSrcRect
   * @param aDstX
   * @param aDstY
   * @param aFlags
   * @return
   */
  TBool DrawBitmap(BViewPort *aViewPort, BBitmap *aSrcBitmap, TRect aSrcRect,
      TInt aDstX, TInt aDstY, TUint32 aFlags = 0);

  /**
   * Draw a rectangle from src bitmap to destination bitmap.
   * Flip/flop/etc. are indicated by the BSprite flags bits passed in.
   * Transparency is considered (e.g. transparent pixels not rendered).
   *
   * @param aViewPort
   * @param aSrcBitmap
   * @param aSrcRect
   * @param aDstX
   * @param aDstY
   * @param aFlags
   * @return
   */
  TBool DrawBitmapTransparent(BViewPort *aViewPort, BBitmap *aSrcBitmap,
      TRect aSrcRect, TInt aDstX, TInt aDstY, TUint32 aFlags = 0);

  /**
   * Draw a text string using a specific font
   * Transparent pixels are considered if aBgColor is -1
   *
   * @param aViewPort
   * @param aStr
   * @param aFont
   * @param aDstX
   * @param aDstY
   * @param aFgColor
   * @param aBgColor
   * @param aLetterSpacing
   * @return
   */
  TBool DrawString(BViewPort *aViewPort, const char *aStr, const BFont *aFont,
      TInt aDstX, TInt aDstY, TUint8 aFgColor, TInt16 aBgColor = -1,
      TInt aLetterSpacing = 0);

  TBool DrawString(BViewPort *aViewPort, const char *aStr, const BFont *aFont,
      TInt aDstX, TInt aDstY, const TRGB &aFgColor, const TRGB &aBgColor,
      TInt aLetterSpacing = 0);

  TBool DrawStringShadow(BViewPort *aViewPort, const char *aStr,
      const BFont *aFont, TInt aDstX, TInt aDstY, TUint8 aFgColor,
      TUint8 aShadowColor, TInt16 aBgColor = -1, TInt aLetterSpacing = 0);

  TBool DrawStringShadow(BViewPort *aViewPort, const char *aStr,
      const BFont *aFont, TInt aDstX, TInt aDstY, TUint8 aFgColor,
      TUint8 aShadowColor, TInt32 aBgColor = -1, TInt aLetterSpacing = 0);

  TBool DrawStringShadow(BViewPort *aViewPort, const char *aStr,
      const BFont *aFont, TInt aDstX, TInt aDstY, const TRGB &aFgColor,
      const TRGB &aShadowColor, const TRGB &aBgColor, TInt aLetterSpacing = 0);

  TUint GetPitch() { return mPitch; }

  TRGB *GetPalette() { return mPalette; }

public:
  TUint8 *mPixels;

protected:
  TBool mROM; // true if mPixels are in ROM
  TRect mDimensions;
  TUint mWidth, mHeight;
  TUint mDepth;
  TUint mColors;          // number of colors
  TInt mTransparentColor; // index of transparent color (#ff00ff) or -1
  TRGB *mPalette;
  TBool mColorsUsed[256]; // ETrue if color in palette is used
};

#endif
