#ifndef BBITMAP_H
#define BBITMAP_H

#include "BBase.h"

const TUint32 DRAW_NORMAL       = 0;
const TUint32 DRAW_FLIPPED      = (1 << 0);
const TUint32 DRAW_FLOPPED      = (1 << 1);
const TUint32 DRAW_ROTATE_RIGHT = (1 << 2);
const TUint32 DRAW_ROTATE_LEFT  = (1 << 3);

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
  friend BResourceManager;
  friend Display;
public:
  BBitmap(TUint aWidth, TUint aHeight, TUint aDepth, TUint16 aMemoryFlag = MEMF_SLOW);

  BBitmap(TAny *aRom, TUint16 aMemoryFlag = MEMF_SLOW);

  virtual ~BBitmap();

public:
  TUint Width() { return mWidth; }

  TUint Height() { return mHeight; }

  TUint Depth() { return mDepth; }

  TInt TransparentColor() { return mTransparentColor; }

  TRect Dimensions() { return mDimensions; }
  TUint8 *GetPixels() {
    return mPixels;
  }

public:
  void Dump();

public:
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

  TRGB &GetColor(TUint8 index) {
    return mPalette[index];
  }

//  void SetPalette(TUint8 index, TUint32 *aPalette, TInt aCount);
public:
  TRGB ReadColor(TInt aX, TInt aY) {
    TUint8 pixel = mPixels[aY * mPitch + aX];
    return mPalette[pixel];
  }

  TUint8 ReadPixel(TInt aX, TInt aY) { return mPixels[aY * mPitch + aX]; }

public:
  void WritePixel(TInt aX, TInt aY, TUint8 aColor) {
    mPixels[aY * mPitch + aX] = aColor;
  }

  void SafeWritePixel(TInt aX, TInt aY, TUint8 aColor) {
    if (mDimensions.PointInRect(aX, aY)) {
      WritePixel(aX, aY, aColor);
    }
  }

public:
  // DRAWING PRIMITIVES
  /**
   * Erase bitmap to specified color (palette index)
   */
  void Clear(TUint8 aColor = 0);

  /**
   * Copy the pixels from one bitmap to another.
   *
   * @param aOther
   */
  void CopyPixels(BBitmap *aOther);


  void DrawFastHLine(BViewPort *aViewPort, TInt aX, TInt aY, TUint aW, TUint8 aColor);

  void DrawFastVLine(BViewPort *aViewPort, TInt aX, TInt aY, TUint aW, TUint8 aColor);

  void DrawLine(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint8 aColor);

  void DrawRect(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint8 aColor);

  void FillRect(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint8 aColor);

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
  TBool
  DrawBitmap(BViewPort *aViewPort, BBitmap *aSrcBitmap, TRect aSrcRect, TInt aDstX, TInt aDstY, TUint32 aFlags = 0);

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
  TBool DrawBitmapTransparent(BViewPort *aViewPort, BBitmap *aSrcBitmap, TRect aSrcRect, TInt aDstX, TInt aDstY,
                              TUint32 aFlags = 0);

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
  TBool DrawString(BViewPort *aViewPort, const char *aStr, const BFont *aFont, TInt aDstX, TInt aDstY, TInt aFgColor,
                   TInt aBgColor = -1, TInt aLetterSpacing = 0);

  TBool DrawStringShadow(BViewPort *aViewPort, const char *aStr, const BFont *aFont, TInt aDstX, TInt aDstY,
                         TInt aFgColor, TInt aShadowColor, TInt aBgColor = -1, TInt aLetterSpacing = 0);

public:
  TUint8 *mPixels;

protected:
  TBool mROM; // true if mPixels are in ROM
  TRect mDimensions;
  TUint mWidth, mHeight;
  TUint mDepth;
  TUint mPitch;
  TUint mColors; // number of colors
  TInt  mTransparentColor;  // index of transparent color (#ff00ff) or -1
  TRGB  *mPalette;
};

#endif
