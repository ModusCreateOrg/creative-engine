#include "BBitmap.h"
#include <string.h>
#include "Panic.h"

#ifndef __XTENSA__

#include <stdio.h>

#endif

BBitmap::BBitmap(TUint aWidth, TUint aHeight, TUint aDepth, TUint16 aMemoryType) {
  mROM              = EFalse;
  mWidth            = aWidth;
  mHeight           = aHeight;
  mDepth            = aDepth;
  mPitch            = mWidth;
  mColors           = 256;
  mPalette          = new TRGB[mColors];
  mPixels           = (TUint8 *) AllocMem(mWidth * mHeight, aMemoryType);
  mTransparentColor = -1;

  mDimensions.x1 = mDimensions.y1 = 0;
  mDimensions.x2 = mWidth - 1;
  mDimensions.y2 = mHeight - 1;
}

/**
 * construct a BBitmap from ROM definition
 */
struct ROMBitmap {
  TUint16 width, height, depth, bytesPerRow, paletteSize;
  TUint8  palette[1];
};

BBitmap::BBitmap(TAny *aROM, TUint16 aMemoryType) {
  ROMBitmap *bmp = (ROMBitmap *) aROM;

  mROM     = ETrue;
  mWidth   = bmp->width;
  mHeight  = bmp->height;
  mDepth   = bmp->depth;
  mPitch   = bmp->bytesPerRow;
  mColors  = bmp->paletteSize;
  mPalette = new TRGB[bmp->paletteSize];
  if (!mPalette) {
    Panic("Cannot allocate mPalette\n");
  }

  mDimensions.x1 = mDimensions.y1 = 0;
  mDimensions.x2 = mWidth - 1;
  mDimensions.y2 = mHeight - 1;

  mTransparentColor = -1;
  TUint8    *ptr = &bmp->palette[0];
  for (TInt i    = 0; i < bmp->paletteSize * 3; i += 3) {
    const TUint8 r = *ptr++;
    const TUint8 g = *ptr++;
    const TUint8 b = *ptr++;
    mPalette[i / 3].Set(r, g, b);
    if (r == 0xff && g == 0x00 && b == 0xff) {
      mTransparentColor = i / 3;
    }
  }

  mPixels = (TUint8 *) AllocMem(mWidth * mHeight, aMemoryType);
  if (!mPixels) {
    Panic("Cannot allocate mPixels\n");
  }
  TUint8    *dst = mPixels;
  for (TInt i    = 0; i < mHeight * mPitch; i++) {
    *dst++ = *ptr++;
  }
}

BBitmap::~BBitmap() {
  if (!mROM) {
    delete[] mPixels;
  }
  delete[] mPalette;
}

void BBitmap::Dump() {
  printf("mRom: %d\n", mROM);
  printf("mWidth: %d, mHeight: %d, mDepth: %d, mPitch: %d\n", mWidth, mHeight, mDepth, mPitch);
  printf("mPixels: %p\n", mPixels);
  printf("mColors: %d, mPalette: %p\n", mColors, mPalette);
  for (TInt c = 0; c < mColors; c++) {
    printf("%3d ", c);
    mPalette[c].Dump();
  }
}

void BBitmap::SetPalette(TRGB aPalette[], TInt aCount) {
  TInt      cnt = MIN(mColors, aCount);
  for (TInt i   = 0; i < cnt; i++) {
    mPalette[i].Set(aPalette[i]);
  }
}

TBool BBitmap::DrawBitmap(BViewPort *aViewPort, BBitmap *aSrcBitmap, TRect aSrcRect, TInt aX, TInt aY,
                          TBool aFlipped, TBool aFlopped, TBool aLeft, TBool aRight) {
  TInt viewPortOffsetX = 0, viewPortOffsetY = 0;

  TRect clipRect;
  if (aViewPort) {
    aViewPort->GetRect(clipRect);
    viewPortOffsetX = aViewPort->mOffsetX;
    viewPortOffsetY = aViewPort->mOffsetY;
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  }

  // Clamp x, y coords
  const TInt clampX = MIN(0, aX);
  const TInt clampY = MIN(0, aY);

  // Calculate clipped width and height
  const TInt clipW = aSrcRect.x1 < 0 ? aSrcRect.x2 : aSrcRect.Width();
  const TInt clipH = aSrcRect.y1 < 0 ? aSrcRect.y2 : aSrcRect.Height();

  // Calculate drawable width and height
  const TInt w = (clipW + clampX) - MAX(0, (clipW + aX) - clipRect.Width() + viewPortOffsetX);
  const TInt h = (clipH + clampY) - MAX(0, (clipH + aY) - clipRect.Height() + viewPortOffsetY);

  // Return if the sprite to be drawn can not be seen
  if (h <= 0) {
    return EFalse;
  }

  // Init source x,y coordinates
  const TInt sx = (aSrcRect.x1 < 0 ? aSrcRect.x1 : aSrcRect.x1 * -1) + clampX;
  const TInt sy = (aSrcRect.y1 < 0 ? aSrcRect.y1 : aSrcRect.y1 * -1) + clampY;

  // Init destination x,y coordinates
  const TInt dx = (aX < 0 ? 0 : aX) + viewPortOffsetX;
  const TInt dy = (aY < 0 ? 0 : aY) + viewPortOffsetY;

  // Calculate sprite delta width and height
  const TInt deltaImageWidth = aX < 0 ? -aSrcRect.Width() + w : aSrcRect.Width() - w;
  const TInt deltaImageHeight = aY < 0 ? -aSrcRect.Height() + h : aSrcRect.Height() - h;

  // Calculate visible width and height to iterate over
  const TInt i = -sy + h;
  const TInt j = -sx + w;

  if (aFlipped) {
    if (aFlopped) {
      // flipped and flopped
      for (TInt yy = -sy, dyy = dy, fsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, fsy--) {
        for (TInt xx = -sx, dxx = dx, fsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, fsx--) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(fsx, fsy);

          // Write pixel values
          WritePixel(dxx, dyy, pix);
        }
      }
    } else {
      // flipped
      for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
        for (TInt xx = -sx, dxx = dx, fsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, fsx--) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(fsx, yy);

          // Write pixel values
          WritePixel(dxx, dyy, pix);
        }
      }
    }
  } else if (aFlopped) {
    // flopped
    for (TInt yy = -sy, dyy = dy, fsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, fsy--) {
      for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
        // Read pixel value from bitmap
        TUint8 pix = aSrcBitmap->ReadPixel(xx, fsy);

        // Write pixel values
        WritePixel(dxx, dyy, pix);
      }
    }
  } else {
    // just draw
    for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
      for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
        // Read pixel value from bitmap
        TUint8 pix = aSrcBitmap->ReadPixel(xx, yy);

        // Write pixel values
        WritePixel(dxx, dyy, pix);
      }
    }
  }

  return ETrue;
}

TBool
BBitmap::DrawSprite(BViewPort *aViewPort, TInt16 aBitmapNumber, TInt aImageNumber, TInt aX, TInt aY, TUint32 aFlags) {
  BBitmap *b      = resourceManager.GetBitmap(aBitmapNumber);
  TInt    bw      = resourceManager.BitmapWidth(aBitmapNumber),
          bh      = resourceManager.BitmapHeight(aBitmapNumber),
          pitch   = b->mWidth / bw,
          viewPortOffsetX = 0,
          viewPortOffsetY = 0;

  TRect imageRect;
  imageRect.x1 = (aImageNumber % pitch) * bw;
  imageRect.x2 = imageRect.x1 + bw - 1;
  imageRect.y1 = (aImageNumber / pitch) * bh;
  imageRect.y2 = imageRect.y1 + bh - 1;

  // Sprite has no transparency
  TInt t = b->mTransparentColor;
  if (t == -1) {
    return DrawBitmap(aViewPort, b, imageRect, aX, aY, TBool(aFlags & SFLAG_FLIP), TBool(aFlags & SFLAG_FLOP),
                      TBool(aFlags & SFLAG_LEFT), TBool(aFlags & SFLAG_RIGHT));
  }

  // Create the viewport
  TRect clipRect;
  if (aViewPort) {
    aViewPort->GetRect(clipRect);
    viewPortOffsetX = aViewPort->mOffsetX;
    viewPortOffsetY = aViewPort->mOffsetY;
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  }

  // Clamp x, y coords
  const TInt clampX = MIN(0, aX);
  const TInt clampY = MIN(0, aY);

  // Calculate clipped width and height
  const TInt clipW = imageRect.x1 < 0 ? imageRect.x2 : imageRect.Width();
  const TInt clipH = imageRect.y1 < 0 ? imageRect.y2 : imageRect.Height();

  // Calculate drawable width and height
  const TInt w = (clipW + clampX) - MAX(0, (clipW + aX) - clipRect.Width() + viewPortOffsetX);
  const TInt h = (clipH + clampY) - MAX(0, (clipH + aY) - clipRect.Height() + viewPortOffsetY);

  // Return if the sprite to be drawn can not be seen
  if (h <= 0) {
    return EFalse;
  }

  // Init source x,y coordinates
  const TInt sx = (imageRect.x1 < 0 ? imageRect.x1 : imageRect.x1 * -1) + clampX;
  const TInt sy = (imageRect.y1 < 0 ? imageRect.y1 : imageRect.y1 * -1) + clampY;

  // Init destination x,y coordinates
  const TInt dx = (aX < 0 ? 0 : aX) + viewPortOffsetX;
  const TInt dy = (aY < 0 ? 0 : aY) + viewPortOffsetY;

  // Calculate sprite delta width and height
  const TInt deltaImageWidth = aX < 0 ? -imageRect.Width() + w : imageRect.Width() - w;
  const TInt deltaImageHeight = aY < 0 ? -imageRect.Height() + h : imageRect.Height() - h;

  // Calculate visible width and height to iterate over
  const TInt i = -sy + h;
  const TInt j = -sx + w;

  if (aFlags & SFLAG_FLIP) {
    if (aFlags & SFLAG_FLOP) {
      // flipped and flopped
      for (TInt yy = -sy, dyy = dy, fsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, fsy--) {
        for (TInt xx = -sx, dxx = dx, fsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, fsx--) {
          // Read pixel value from bitmap
          TUint8 pix = b->ReadPixel(fsx, fsy);

          // Write non-transparent pixel values
          if (pix != t) {
            WritePixel(dxx, dyy, pix);
          }
        }
      }
    } else {
      // flipped
      for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
        for (TInt xx = -sx, dxx = dx, fsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, fsx--) {
          // Read pixel value from bitmap
          TUint8 pix = b->ReadPixel(fsx, yy);

          // Write non-transparent pixel values
          if (pix != t) {
            WritePixel(dxx, dyy, pix);
          }
        }
      }
    }
  } else if (aFlags & SFLAG_FLOP) {
    // flopped
    for (TInt yy = -sy, dyy = dy, fsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, fsy--) {
      for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
        // Read pixel value from bitmap
        TUint8 pix = b->ReadPixel(xx, fsy);

        // Write non-transparent pixel values
        if (pix != t) {
          WritePixel(dxx, dyy, pix);
        }
      }
    }
  } else {
    // just draw
    for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
      for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
        // Read pixel value from bitmap
        TUint8 pix = b->ReadPixel(xx, yy);

        // Write non-transparent pixel values
        if (pix != t) {
          WritePixel(dxx, dyy, pix);
        }
      }
    }
  }

  return ETrue;
}

void BBitmap::Clear(TUint8 aColor) {
  memset(mPixels, aColor, mPitch * mHeight);
}

void BBitmap::DrawLine(TUint8 aColor, TInt aX1, TInt aY1, TInt aX2, TInt aY2) {
  // TODO: Jay implement this
}

void BBitmap::DrawRect(TUint8 aColor, TInt aX1, TInt aY1, TInt aX2, TInt aY2) {
  // TODO: Jay implement this
}
