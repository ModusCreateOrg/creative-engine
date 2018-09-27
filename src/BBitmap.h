#ifndef BBITMAP_H
#define BBITMAP_H

#include "BBase.h"

/**
 * Bitmaps.
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

class BResourceManager;

class Display;

class BViewPort;

class BFont;

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

  TRect Dimensions() { return mDimensions; }

public:
  void Dump();

public:
  /**
   * Set the BBitmap's palette from an array of TRGB
   *
   * @param aPalette
   * @param aCount
   */
  void SetPalette(TRGB aPalette[], TInt aCount = 256);

  /**
   * Set palette by copying palette from another BBitmap
   * @param aBitmap
   * @param aCount
   */
  void SetPalette(BBitmap *aBitmap, TInt aCount = 256) {
    SetPalette(aBitmap->mPalette, aCount);
  }

  void SetColor(TUint8 index, TUint8 r, TUint8 g, TUint8 b) {
    mPalette[index].Set(r, g, b);
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

  // TODO: Jay implement these?
  void DrawLine(TUint8 aColor, TInt aX1, TInt aY1, TInt aX2, TInt aY2);

  void DrawRect(TUint8 aColor, TInt aX1, TInt aY1, TInt aX2, TInt aY2);

public:
  /** Draw a rectangle from src bitmap to destination bitmap.
   *
   * @param aSrcBitmap
   * @param aSrcRect
   * @param aDstX
   * @param aDstY
   * @param aFlipped
   * @param aFlopped
   * @param aLeft
   * @param aRight
   * @return
   */
  TBool DrawBitmap(BViewPort *aViewPort, BBitmap *aSrcBitmap, TRect aSrcRect, TInt aDstX, TInt aDstY,
                   TBool aFlipped = EFalse, TBool aFlopped = EFalse, TBool aLeft = EFalse, TBool aRight = EFalse);

  /**
   * Draw a sprite to this bitmap.  Flip/flop/etc. are indicated by the BSprite flags bits passed in.
   * Transparency is considered (e.g. transparent pixels not rendered).
   *
   * @param aSrcBitmap
   * @param aSrcRect
   * @param aDstX
   * @param aDstY
   * @param aFlags
   * @return
   */
  TBool DrawSprite(BViewPort *aViewPort, TInt16 aBitmapSlot, TInt aImageNumber, TInt aX, TInt aY, TUint32 aFlags = 0);

public:
  TBool DrawString(BViewPort *aViewPort, BFont *aFont, TInt aDstX, TInt aDstY, const char *aString);

protected:
  TBool  mROM; // true if mPixels are in ROM
  TRect  mDimensions;
  TUint  mWidth, mHeight;
  TUint  mDepth;
  TUint  mPitch;
  TUint8 *mPixels;
  TUint  mColors; // number of colors
  TInt   mTransparentColor;  // index of transparent color (#ff00ff) or -1
  TRGB   *mPalette;
};

#endif
