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
  printf("mRom: %d\n:", mROM);
  printf("mWidth: %d, mHeight: %d, mDepth: %d, mPitch: %d\n", mWidth, mHeight, mDepth, mPitch);
  printf("mPixels: %p\n", mPixels);
  printf("mColors: %d, mPalette: %p\n", mColors, mPalette);
}

void BBitmap::SetPalette(TRGB aPalette[], TInt aCount) {
  TInt      cnt = MIN(mColors, aCount);
  for (TInt i   = 0; i < cnt; i++) {
    mPalette[i].Set(aPalette[i]);
  }
}

TBool BBitmap::DrawBitmap(BViewPort *aViewPort, BBitmap *aSrcBitmap, TRect aSrcRect, TInt aX, TInt aY,
                          TBool aFlipped, TBool aFlopped, TBool aLeft, TBool aRight) {
  TRect clipRect;
  if (aViewPort) {
    aViewPort->GetRect(clipRect);
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  }

  // TODO implement flipped, flopped, left, right
  TBool clipped = ETrue;
  // TODO: optimize this.  We're iterating in the loop over bits that are
  // TODO: off the edge of the destingation bitmap!
  TInt  w       = aSrcRect.Width(), h = aSrcRect.Height();

  for (TInt y = 0; y < h; y++) {
    const TInt sy = y + aSrcRect.y1;
    if (sy < 0 || sy > Height()) {
      continue;
    }
    const TInt dy = y + aY;
    if (dy < clipRect.y1 || dy > clipRect.y2) {
      continue;
    }
    for (TInt x = 0; x < w; x++) {
      const TInt sx = x + aSrcRect.x1;
      if (sx < 0 || sx > Width()) {
        continue;
      }
      const TInt dx = x + aX;
      if (dx < clipRect.x1 || dx > clipRect.x2) {
        continue;
      }
      TUint8 pix = aSrcBitmap->ReadPixel(sx, sy);
      WritePixel(dx, dy, pix);
      clipped = EFalse;
    }
  }
  return clipped;
}

TBool
BBitmap::DrawSprite(BViewPort *aViewPort, TInt16 aBitmapNumber, TInt aImageNumber, TInt aX, TInt aY, TUint32 aFlags) {
  BBitmap *b    = resourceManager.GetBitmap(aBitmapNumber);
  TInt    bw    = resourceManager.BitmapWidth(aBitmapNumber),
          bh    = resourceManager.BitmapHeight(aBitmapNumber),
          pitch = b->mWidth / bw;

  TRect imageRect;
  imageRect.x1 = (aImageNumber % pitch) * bw;
  imageRect.x2 = imageRect.x1 + bw - 1;
  imageRect.y1 = (aImageNumber / pitch) * bh;
  imageRect.y2 = imageRect.y1 + bh - 1;

  TInt t = b->mTransparentColor;
  if (t == -1) {
    return DrawBitmap(aViewPort, b, imageRect, aX, aY, TBool(aFlags & SFLAG_FLIP), TBool(aFlags & SFLAG_FLOP),
                      TBool(aFlags & SFLAG_LEFT), TBool(aFlags & SFLAG_RIGHT));
  }

  TRect clipRect;
  if (aViewPort) {
    aViewPort->GetRect(clipRect);
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  }

  TBool clipped = ETrue;
  TInt  w       = imageRect.Width(), h = imageRect.Height();
  if (aFlags & SFLAG_FLIP) {
    if (aFlags & SFLAG_FLOP) {
      // flipped and flopped
      for (TInt y = 0; y < h; y++) {
        const TInt sy = (h - y) + imageRect.y1;
        if (sy < 0 || sy > Height()) {
          continue;
        }
        const TInt dy = y + aY;
        if (dy < clipRect.y1 || dy > clipRect.y2) {
          continue;
        }
        for (TInt x = 0; x < w; x++) {
          const TInt sx = (w - x) + imageRect.x1;
          if (sx < 0 || sx > Width()) {
            continue;
          }
          const TInt dx = x + aX + (w - 1);
          if (dx < clipRect.x1 || dx > clipRect.x2) {
            continue;
          }
          TUint8 pix = b->ReadPixel(sx, sy);
          if (pix != t) {
            WritePixel(dx, dy, pix);
          }
          clipped = EFalse;
        }
      }
    } else {
      // flipped
      for (TInt y = 0; y < h; y++) {
        const TInt sy = y + imageRect.y1;
        if (sy < 0 || sy > Height()) {
          continue;
        }
        const TInt dy = y + aY;
        if (dy < clipRect.y1 || dy > clipRect.y2) {
          continue;
        }
        for (TInt x = 0; x < w; x++) {
          const TInt sx = (w - x) + imageRect.x1;
          if (sx < 0 || sx > Width()) {
            continue;
          }
          const TInt dx = x + aX;
          if (dx < clipRect.x1 || dx > clipRect.x2) {
            continue;
          }
          TUint8 pix = b->ReadPixel(sx, sy);
          if (pix != t) {
            WritePixel(dx, dy, pix);
          }
          clipped = EFalse;
        }
      }
    }
  } else if (aFlags & SFLAG_FLOP) {
    // flopped
    for (TInt y = 0; y < h; y++) {
      const TInt sy = (h - y) + imageRect.y1;
      if (sy < 0 || sy > Height()) {
        continue;
      }
      const TInt dy = y + aY;
      if (dy < clipRect.y1 || dy > clipRect.y2) {
        continue;
      }
      for (TInt x = 0; x < w; x++) {
        const TInt sx = x + imageRect.x1;
        if (sx < 0 || sx > Width()) {
          continue;
        }
        const TInt dx = x + aX;
        if (dx < clipRect.x1 || dx > clipRect.x2) {
          continue;
        }
        TUint8 pix = b->ReadPixel(sx, sy);
        if (pix != t) {
          WritePixel(dx, dy, pix);
        }
        clipped = EFalse;
      }
    }
  } else {
    // just draw
    for (TInt y = 0; y < h; y++) {
      const TInt sy = y + imageRect.y1;
      if (sy < 0 || sy > Height()) {
        continue;
      }
      const TInt dy = y + aY;
      if (dy < clipRect.y1 || dy > clipRect.y2) {
        continue;
      }
      for (TInt x = 0; x < w; x++) {
        const TInt sx = x + imageRect.x1;
        if (sx < 0 || sx > Width()) {
          continue;
        }
        const TInt dx = x + aX;
        if (dx < clipRect.x1 || dx > clipRect.x2) {
          continue;
        }
        TUint8 pix = b->ReadPixel(sx, sy);
        if (pix != t) {
          WritePixel(dx, dy, pix);
        }
        clipped = EFalse;
      }
    }
  }
  return !clipped;
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
