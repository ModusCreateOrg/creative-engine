#include "BBitmap.h"
#include <cstring>
#include "Panic.h"
#include "BFont.h"
#include <strings.h>
#include <cmath>

#ifndef __XTENSA__

#include <cstdio>

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
  auto *bmp = (ROMBitmap *) aROM;

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
  FreeMem(mPixels);
  mPixels = ENull;
  delete[] mPalette;
  mPalette = ENull;
}

void BBitmap::Dump() {
#ifndef PRODUCTION
#if (defined(__XTENSA__) && defined(DEBUGME)) || !defined(__XTENSA__)
  printf("mRom: %d\n", mROM);
  printf("mWidth: %d, mHeight: %d, mDepth: %d, mPitch: %d\n", mWidth, mHeight, mDepth, mPitch);
  printf("mPixels: %s\n", mPixels);
  printf("mColors: %d, mPalette: %p\n", mColors, mPalette);
  for (TInt c = 0; c < mColors; c++) {
    printf("%3d ", c);
    mPalette[c].Dump();
  }
#endif
#endif
}

void BBitmap::SetPalette(TRGB aPalette[], TInt aIndex, TInt aCount) {
  TInt      cnt = MIN(mColors, aCount);
  for (TInt i   = 0; i < cnt; i++) {
    mPalette[aIndex + i].Set(aPalette[aIndex + i]);
  }
}

TBool BBitmap::DrawBitmap(BViewPort *aViewPort, BBitmap *aSrcBitmap, TRect aSrcRect, TInt aX, TInt aY, TUint32 aFlags) {
  TInt viewPortOffsetX = 0, viewPortOffsetY = 0;

  TRect clipRect;
  if (aViewPort) {
    aViewPort->GetRect(clipRect);
    viewPortOffsetX = TInt(round(aViewPort->mOffsetX));
    viewPortOffsetY = TInt(round(aViewPort->mOffsetY));
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
  }

  // Clamp x, y coords
  const TInt clampX = MIN(0, aX);
  const TInt clampY = MIN(0, aY);

  // Calculate clipped width and height
  const TInt clipW = aSrcRect.x1 < 0 ? aSrcRect.x2 : aSrcRect.Width();
  const TInt clipH = aSrcRect.y1 < 0 ? aSrcRect.y2 : aSrcRect.Height();

  // Calculate drawable width and height
  const TInt w = TBool(aFlags & DRAW_ROTATE_RIGHT) ^ TBool(aFlags & DRAW_ROTATE_LEFT)
                 ? (clipH + clampX) - MAX(0, (clipH + aX) - clipRect.Width() + viewPortOffsetX)
                 : (clipW + clampX) - MAX(0, (clipW + aX) - clipRect.Width() + viewPortOffsetX);

  const TInt h        = TBool(aFlags & DRAW_ROTATE_RIGHT) ^ TBool(aFlags & DRAW_ROTATE_LEFT)
                        ? (clipW + clampY) - MAX(0, (clipW + aY) - clipRect.Height() + viewPortOffsetY)
                        : (clipH + clampY) - MAX(0, (clipH + aY) - clipRect.Height() + viewPortOffsetY);

  // Return if the sprite to be drawn can not be seen
  if (w < 1 || h < 1) {
    return EFalse;
  }

  TUint8        *pixels;
  const TUint32 pitch = this->mPitch;

  // Init source x,y coordinates
  const TInt sx = (aSrcRect.x1 < 0 ? aSrcRect.x1 : aSrcRect.x1 * -1) + clampX;
  const TInt sy = (aSrcRect.y1 < 0 ? aSrcRect.y1 : aSrcRect.y1 * -1) + clampY;

  // Init destination x,y coordinates
  const TInt dx = (aX < 0 ? 0 : aX) + viewPortOffsetX;
  const TInt dy = (aY < 0 ? 0 : aY) + viewPortOffsetY;

  // Calculate sprite delta width and height
  const TInt deltaImageWidth = TBool(aFlags & DRAW_ROTATE_RIGHT) ^ TBool(aFlags & DRAW_ROTATE_LEFT)
                               ? (aX < 0 ? -aSrcRect.Height() + w : aSrcRect.Height() - w) - 1
                               : (aX < 0 ? -aSrcRect.Width() + w : aSrcRect.Width() - w) - 1;

  const TInt deltaImageHeight = TBool(aFlags & DRAW_ROTATE_RIGHT) ^ TBool(aFlags & DRAW_ROTATE_LEFT)
                                ? (aY < 0 ? -aSrcRect.Width() + h : aSrcRect.Width() - h) - 1
                                : (aY < 0 ? -aSrcRect.Height() + h : aSrcRect.Height() - h) - 1;

  // Calculate visible width and height to iterate over
  const TInt i = -sy + h;
  const TInt j = -sx + w;

  if (aFlags & DRAW_FLIPPED) {
    if (aFlags & DRAW_FLOPPED) {
      if (aFlags & DRAW_ROTATE_LEFT && !(aFlags & DRAW_ROTATE_RIGHT)) {
        // flip and flop and rotate left
        for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
          pixels = &this->mPixels[dyy * pitch + dx];

          for (TInt xx = -sx, dxx = dx, rsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, rsx--) {
            // Read pixel value from bitmap
            TUint8 pix = aSrcBitmap->ReadPixel(yy, rsx);

            // Write pixel values
            *pixels++ = pix;
          }
        }
      } else if (aFlags & DRAW_ROTATE_RIGHT && !(aFlags & DRAW_ROTATE_LEFT)) {
        // flip and flop and rotate right
        for (TInt yy = -sy, dyy = dy, rsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, rsy--) {
          pixels = &this->mPixels[dyy * pitch + dx];

          for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
            // Read pixel value from bitmap
            TUint8 pix = aSrcBitmap->ReadPixel(rsy, xx);

            // Write pixel values
            *pixels++ = pix;
          }
        }
      } else {
        // flipped and flopped
        for (TInt yy = -sy, dyy = dy, fsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, fsy--) {
          pixels = &this->mPixels[dyy * pitch + dx];

          for (TInt xx = -sx, dxx = dx, fsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, fsx--) {
            // Read pixel value from bitmap
            TUint8 pix = aSrcBitmap->ReadPixel(fsx, fsy);

            // Write pixel values
            *pixels++ = pix;
          }
        }
      }
    } else {
      if (aFlags & DRAW_ROTATE_LEFT && !(aFlags & DRAW_ROTATE_RIGHT)) {
        // flip and rotate left
        for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
          pixels = &this->mPixels[dyy * pitch + dx];

          for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
            // Read pixel value from bitmap
            TUint8 pix = aSrcBitmap->ReadPixel(yy, xx);

            // Write pixel values
            *pixels++ = pix;
          }
        }
      } else if (aFlags & DRAW_ROTATE_RIGHT && !(aFlags & DRAW_ROTATE_LEFT)) {
        // flip and rotate right
        for (TInt yy = -sy, dyy = dy, rsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, rsy--) {
          pixels = &this->mPixels[dyy * pitch + dx];

          for (TInt xx = -sx, dxx = dx, fsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, fsx--) {
            // Read pixel value from bitmap
            TUint8 pix = aSrcBitmap->ReadPixel(rsy, fsx);

            // Write pixel values
            *pixels++ = pix;
          }
        }
      } else {
        // flipped
        for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
          pixels = &this->mPixels[dyy * pitch + dx];

          for (TInt xx = -sx, dxx = dx, fsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, fsx--) {
            // Read pixel value from bitmap
            TUint8 pix = aSrcBitmap->ReadPixel(fsx, yy);

            // Write pixel values
            *pixels++ = pix;
          }
        }
      }
    }
  } else if (aFlags & DRAW_FLOPPED) {
    if (aFlags & DRAW_ROTATE_LEFT && !(aFlags & DRAW_ROTATE_RIGHT)) {
      // flop and rotate left
      for (TInt yy = -sy, dyy = dy, fsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, fsy--) {
        pixels = &this->mPixels[dyy * pitch + dx];

        for (TInt xx = -sx, dxx = dx, rsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, rsx--) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(fsy, rsx);

          // Write pixel values
          *pixels++ = pix;
        }
      }
    } else if (aFlags & DRAW_ROTATE_RIGHT && !(aFlags & DRAW_ROTATE_LEFT)) {
      // flop and rotate right
      for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
        pixels = &this->mPixels[dyy * pitch + dx];

        for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(yy, xx);

          // Write pixel values
          *pixels++ = pix;
        }
      }
    } else {
      // flopped
      for (TInt yy = -sy, dyy = dy, fsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, fsy--) {
        pixels = &this->mPixels[dyy * pitch + dx];

        for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(xx, fsy);

          // Write pixel values
          *pixels++ = pix;
        }
      }
    }
  } else {
    if (aFlags & DRAW_ROTATE_LEFT && !(aFlags & DRAW_ROTATE_RIGHT)) {
      // rotate left
      for (TInt yy = -sy, dyy = dy, rsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, rsy--) {
        pixels = &this->mPixels[dyy * pitch + dx];

        for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(rsy, xx);

          // Write pixel values
          *pixels++ = pix;
        }
      }
    } else if (aFlags & DRAW_ROTATE_RIGHT && !(aFlags & DRAW_ROTATE_LEFT)) {
      // rotate right
      for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
        pixels = &this->mPixels[dyy * pitch + dx];

        for (TInt xx = -sx, dxx = dx, rsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, rsx--) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(yy, rsx);

          // Write pixel values
          *pixels++ = pix;
        }
      }
    } else {
      // just draw
      for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
        pixels = &this->mPixels[dyy * pitch + dx];

        for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(xx, yy);

          // Write pixel values
          *pixels++ = pix;
        }
      }
    }
  }

  return ETrue;
}

void BBitmap::CopyPixels(BBitmap *aOther) {
  if (aOther->mWidth != mWidth || aOther->mHeight != mHeight) {
#ifndef PRODUCTION
#if (defined(__XTENSA__) && defined(DEBUGME)) || !defined(__XTENSA__)
    printf("CopyPixels: other bitmap has different dimensions\n");
#endif
#endif
    return;
  }
  memcpy(mPixels, aOther->mPixels, mWidth * mHeight);
}

TBool BBitmap::DrawBitmapTransparent(BViewPort *aViewPort, BBitmap *aSrcBitmap, TRect aSrcRect, TInt aX, TInt aY,
                                     TUint32 aFlags) {
  TInt t               = aSrcBitmap->mTransparentColor,
       viewPortOffsetX = 0,
       viewPortOffsetY = 0;

  // Create the viewport
  TRect clipRect;
  if (aViewPort) {
    aViewPort->GetRect(clipRect);
    viewPortOffsetX = TInt(round(aViewPort->mOffsetX));
    viewPortOffsetY = TInt(round(aViewPort->mOffsetY));
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
  }

  // Clamp x, y coords
  const TInt clampX = MIN(0, aX);
  const TInt clampY = MIN(0, aY);

  // Calculate clipped width and height
  const TInt clipW = aSrcRect.x1 < 0 ? aSrcRect.x2 : aSrcRect.Width();
  const TInt clipH = aSrcRect.y1 < 0 ? aSrcRect.y2 : aSrcRect.Height();

  // Calculate drawable width and height
  const TInt w = TBool(aFlags & DRAW_ROTATE_RIGHT) ^ TBool(aFlags & DRAW_ROTATE_LEFT)
                 ? (clipH + clampX) - MAX(0, (clipH + aX) - clipRect.Width() + viewPortOffsetX)
                 : (clipW + clampX) - MAX(0, (clipW + aX) - clipRect.Width() + viewPortOffsetX);

  const TInt h        = TBool(aFlags & DRAW_ROTATE_RIGHT) ^ TBool(aFlags & DRAW_ROTATE_LEFT)
                        ? (clipW + clampY) - MAX(0, (clipW + aY) - clipRect.Height() + viewPortOffsetY)
                        : (clipH + clampY) - MAX(0, (clipH + aY) - clipRect.Height() + viewPortOffsetY);

  // Return if the sprite to be drawn can not be seen
  if (w < 1 || h < 1) {
    return EFalse;
  }

  TUint8        *pixels;
  const TUint32 pitch = this->mPitch;

  // Init source x,y coordinates
  const TInt sx = (aSrcRect.x1 < 0 ? aSrcRect.x1 : aSrcRect.x1 * -1) + clampX;
  const TInt sy = (aSrcRect.y1 < 0 ? aSrcRect.y1 : aSrcRect.y1 * -1) + clampY;

  // Init destination x,y coordinates
  const TInt dx = (aX < 0 ? 0 : aX) + viewPortOffsetX;
  const TInt dy = (aY < 0 ? 0 : aY) + viewPortOffsetY;

  // Calculate sprite delta width and height
  const TInt deltaImageWidth = TBool(aFlags & DRAW_ROTATE_RIGHT) ^ TBool(aFlags & DRAW_ROTATE_LEFT)
                               ? (aX < 0 ? -aSrcRect.Height() + w : aSrcRect.Height() - w) - 1
                               : (aX < 0 ? -aSrcRect.Width() + w : aSrcRect.Width() - w) - 1;

  const TInt deltaImageHeight = TBool(aFlags & DRAW_ROTATE_RIGHT) ^ TBool(aFlags & DRAW_ROTATE_LEFT)
                                ? (aY < 0 ? -aSrcRect.Width() + h : aSrcRect.Width() - h) - 1
                                : (aY < 0 ? -aSrcRect.Height() + h : aSrcRect.Height() - h) - 1;

  // Calculate visible width and height to iterate over
  const TInt i = -sy + h;
  const TInt j = -sx + w;

  if (aFlags & DRAW_FLIPPED) {
    if (aFlags & DRAW_FLOPPED) {
      if (aFlags & DRAW_ROTATE_LEFT && !(aFlags & DRAW_ROTATE_RIGHT)) {
        // flip and flop and rotate left
        for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
          pixels = &this->mPixels[dyy * pitch + dx];

          for (TInt xx = -sx, dxx = dx, rsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, rsx--) {
            // Read pixel value from bitmap
            TUint8 pix = aSrcBitmap->ReadPixel(yy, rsx);

            // Write non-transparent pixel values
            if (pix != t) {
              *pixels = pix;
            }

            // Increment pointer either way
            pixels++;
          }
        }
      } else if (aFlags & DRAW_ROTATE_RIGHT && !(aFlags & DRAW_ROTATE_LEFT)) {
        // flip and flop and rotate right
        for (TInt yy = -sy, dyy = dy, rsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, rsy--) {
          pixels = &this->mPixels[dyy * pitch + dx];

          for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
            // Read pixel value from bitmap
            TUint8 pix = aSrcBitmap->ReadPixel(rsy, xx);

            // Write non-transparent pixel values
            if (pix != t) {
              *pixels = pix;
            }

            // Increment pointer either way
            pixels++;
          }
        }
      } else {
        // flipped and flopped
        for (TInt yy = -sy, dyy = dy, fsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, fsy--) {
          pixels = &this->mPixels[dyy * pitch + dx];

          for (TInt xx = -sx, dxx = dx, fsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, fsx--) {
            // Read pixel value from bitmap
            TUint8 pix = aSrcBitmap->ReadPixel(fsx, fsy);

            // Write non-transparent pixel values
            if (pix != t) {
              *pixels = pix;
            }

            // Increment pointer either way
            pixels++;
          }
        }
      }
    } else {
      if (aFlags & DRAW_ROTATE_LEFT && !(aFlags & DRAW_ROTATE_RIGHT)) {
        // flip and rotate left
        for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
          pixels = &this->mPixels[dyy * pitch + dx];

          for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
            // Read pixel value from bitmap
            TUint8 pix = aSrcBitmap->ReadPixel(yy, xx);

            // Write non-transparent pixel values
            if (pix != t) {
              *pixels = pix;
            }

            // Increment pointer either way
            pixels++;
          }
        }
      } else if (aFlags & DRAW_ROTATE_RIGHT && !(aFlags & DRAW_ROTATE_LEFT)) {
        // flip and rotate right
        for (TInt yy = -sy, dyy = dy, rsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, rsy--) {
          pixels = &this->mPixels[dyy * pitch + dx];

          for (TInt xx = -sx, dxx = dx, fsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, fsx--) {
            // Read pixel value from bitmap
            TUint8 pix = aSrcBitmap->ReadPixel(rsy, fsx);

            // Write non-transparent pixel values
            if (pix != t) {
              *pixels = pix;
            }

            // Increment pointer either way
            pixels++;
          }
        }
      } else {
        // flipped
        for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
          pixels = &this->mPixels[dyy * pitch + dx];

          for (TInt xx = -sx, dxx = dx, fsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, fsx--) {
            // Read pixel value from bitmap
            TUint8 pix = aSrcBitmap->ReadPixel(fsx, yy);

            // Write non-transparent pixel values
            if (pix != t) {
              *pixels = pix;
            }

            // Increment pointer either way
            pixels++;
          }
        }
      }
    }
  } else if (aFlags & DRAW_FLOPPED) {
    if (aFlags & DRAW_ROTATE_LEFT && !(aFlags & DRAW_ROTATE_RIGHT)) {
      // flop and rotate left
      for (TInt yy = -sy, dyy = dy, fsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, fsy--) {
        pixels = &this->mPixels[dyy * pitch + dx];

        for (TInt xx = -sx, dxx = dx, rsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, rsx--) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(fsy, rsx);

          // Write non-transparent pixel values
          if (pix != t) {
            *pixels = pix;
          }

          // Increment pointer either way
          pixels++;
        }
      }
    } else if (aFlags & DRAW_ROTATE_RIGHT && !(aFlags & DRAW_ROTATE_LEFT)) {
      // flop and rotate right
      for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
        pixels = &this->mPixels[dyy * pitch + dx];

        for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(yy, xx);

          // Write non-transparent pixel values
          if (pix != t) {
            *pixels = pix;
          }

          // Increment pointer either way
          pixels++;
        }
      }
    } else {
      // flopped
      for (TInt yy = -sy, dyy = dy, fsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, fsy--) {
        pixels = &this->mPixels[dyy * pitch + dx];

        for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(xx, fsy);

          // Write non-transparent pixel values
          if (pix != t) {
            *pixels = pix;
          }

          // Increment pointer either way
          pixels++;
        }
      }
    }
  } else {
    if (aFlags & DRAW_ROTATE_LEFT && !(aFlags & DRAW_ROTATE_RIGHT)) {
      // rotate left
      for (TInt yy = -sy, dyy = dy, rsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, rsy--) {
        pixels = &this->mPixels[dyy * pitch + dx];

        for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(rsy, xx);

          // Write non-transparent pixel values
          if (pix != t) {
            *pixels = pix;
          }

          // Increment pointer either way
          pixels++;
        }
      }
    } else if (aFlags & DRAW_ROTATE_RIGHT && !(aFlags & DRAW_ROTATE_LEFT)) {
      // rotate right
      for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
        pixels = &this->mPixels[dyy * pitch + dx];

        for (TInt xx = -sx, dxx = dx, rsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, rsx--) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(yy, rsx);

          // Write non-transparent pixel values
          if (pix != t) {
            *pixels = pix;
          }

          // Increment pointer either way
          pixels++;
        }
      }
    } else {
      // just draw
      for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
        pixels = &this->mPixels[dyy * pitch + dx];

        for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(xx, yy);

          // Write non-transparent pixel values
          if (pix != t) {
            *pixels = pix;
          }

          // Increment pointer either way
          pixels++;
        }
      }
    }
  }

  return ETrue;
}

TBool BBitmap::DrawStringShadow(BViewPort *aViewPort, const char *aStr, const BFont *aFont, TInt aX, TInt aY, TInt aFgColor, TInt aShadowColor, TInt aBgColor, TInt aLetterSpacing) {
  if (!DrawString(aViewPort, aStr, aFont, aX, aY, aShadowColor, aBgColor, aLetterSpacing)) {
    return EFalse;
  }
  return DrawString(aViewPort, aStr, aFont, aX-1, aY-1, aFgColor, aBgColor, aLetterSpacing);
}

TBool BBitmap::DrawString(BViewPort *aViewPort, const char *aStr, const BFont *aFont, TInt aX, TInt aY, TInt aFgColor,
                          TInt aBgColor, TInt aLetterSpacing) {
  TInt viewPortOffsetX      = 0, viewPortOffsetY = 0;

  // Create the viewport
  TRect clipRect;
  if (aViewPort) {
    aViewPort->GetRect(clipRect);
    viewPortOffsetX = TInt(round(aViewPort->mOffsetX));
    viewPortOffsetY = TInt(round(aViewPort->mOffsetY));
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
  }

  TUint8        *pixels;
  TBool         drawn       = false;
  BBitmap       *fontBitmap = aFont->mBitmap;
  const TUint   fontWidth   = aFont->mWidth;
  const TUint   fontHeight  = aFont->mHeight;
  const TInt    charOffset  = fontWidth + aLetterSpacing;
  const TUint32 pitch       = this->mPitch;
  const TBool   drawBg      = aBgColor != -1;

  const TInt clipW = fontWidth;
  const TInt clipH = fontHeight;

  while (*aStr) {
    // Clamp x, y coords
    const TInt clampX = MIN(0, aX);
    const TInt clampY = MIN(0, aY);

    // Calculate drawable width and height
    const TInt w = (clipW + clampX) - MAX(0, (clipW + aX) - clipRect.Width() + viewPortOffsetX);
    const TInt h = (clipH + clampY) - MAX(0, (clipH + aY) - clipRect.Height() + viewPortOffsetY);

    // Return if the string to be drawn can not be seen
    if (w < 1 || h < 1) {
      drawn |= EFalse;
      aStr++;
      aX += charOffset;
      continue;
    }

    // Init destination x,y coordinates
    const TInt dx = (aX < 0 ? 0 : aX) + viewPortOffsetX;
    TInt       dy = (aY < 0 ? 0 : aY) + viewPortOffsetY;

    // Get character
    const char c   = *aStr++;
    const TInt row = c / 16;
    const TInt col = c % 16;

    // Init source x,y coordinates
    const TInt sx = col * -fontWidth + clampX;
    const TInt sy = row * -fontHeight + clampY;

    // Calculate visible width and height to iterate over
    const TInt i = -sy + h;
    const TInt j = -sx + w;

    for (TInt yy = -sy; yy < i; yy++) {
      pixels = &this->mPixels[dy++ * pitch + dx];

      for (TInt xx = -sx; xx < j; xx++) {
        // Read pixel value from bitmap
        TUint8 pix = fontBitmap->ReadPixel(xx, yy);

        // Write background and foreground pixels
        if (pix == 0) {
          if (drawBg) {
            *pixels = TUint8(aBgColor);
          }
        } else {
          *pixels = aFgColor;
        }

        pixels++;
      }
    }

    aX += charOffset;
    drawn |= ETrue;
  }

  return drawn;
}

void BBitmap::Clear(TUint8 aColor) {
  memset(mPixels, aColor, mPitch * mHeight);
}

void BBitmap::DrawFastHLine(BViewPort *aViewPort, TInt aX, TInt aY, TUint aW, TUint8 aColor) {
  // Initial viewport offset
  TInt viewPortOffsetX = 0;
  TInt viewPortOffsetY = 0;

  // Create the viewport
  TRect clipRect;
  if (aViewPort) {
    aViewPort->GetRect(clipRect);
    viewPortOffsetX = TInt(round(aViewPort->mOffsetX));
    viewPortOffsetY = TInt(round(aViewPort->mOffsetY));
    aX += viewPortOffsetX;
    aY += viewPortOffsetY;
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
  }

  // Store viewport width/height
  const TInt clipRectWidth  = clipRect.Width();
  const TInt clipRectHeight = clipRect.Height();

  // Do y bounds checks
  if (aY < viewPortOffsetY || aY >= clipRectHeight) {
    return;
  }

  // last x point + 1
  TInt xEnd = aX + aW;

  // Check if the entire line is not on the display
  if (xEnd <= viewPortOffsetX || aX >= clipRectWidth) {
    return;
  }

  // Don't start before the left edge
  if (aX < viewPortOffsetX) {
    aX = viewPortOffsetX;
  }

  // Don't end past the right edge
  if (xEnd > clipRectWidth) {
    xEnd = clipRectWidth;
  }

  // calculate actual width (even if unchanged)
  aW = TUint(xEnd - aX);

  TUint8 *pixels = &this->mPixels[aY * this->mPitch + aX];

  while (aW > 3) {
    *pixels++ = aColor;
    *pixels++ = aColor;
    *pixels++ = aColor;
    *pixels++ = aColor;
    aW -= 4;
  }

  while (aW > 0) {
    *pixels++ = aColor;
    aW--;
  }
}

void BBitmap::DrawFastVLine(BViewPort *aViewPort, TInt aX, TInt aY, TUint aH, TUint8 aColor) {
  // Initial viewport offset
  TInt viewPortOffsetX = 0;
  TInt viewPortOffsetY = 0;

  // Create the viewport
  TRect clipRect;
  if (aViewPort) {
    aViewPort->GetRect(clipRect);
    viewPortOffsetX = TInt(round(aViewPort->mOffsetX));
    viewPortOffsetY = TInt(round(aViewPort->mOffsetY));
    aX += viewPortOffsetX;
    aY += viewPortOffsetY;
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
  }

  // Store viewport width/height
  const TInt clipRectWidth  = clipRect.Width();
  const TInt clipRectHeight = clipRect.Height();

  // Do x bounds checks
  if (aX < viewPortOffsetX || aX >= clipRectWidth) {
    return;
  }

  // last y point + 1
  TInt yEnd = aY + aH;

  // Check if the entire line is not on the display
  if (yEnd <= viewPortOffsetY || aY >= clipRectHeight) {
    return;
  }

  // Don't start before the top edge
  if (aY < viewPortOffsetY) {
    aY = viewPortOffsetY;
  }

  // Don't end past the bottom edge
  if (yEnd > clipRectHeight) {
    yEnd = clipRectHeight;
  }

  // calculate actual height (even if unchanged)
  aH = TUint(yEnd - aY);

  const TUint32 pitch   = this->mPitch;
  TUint8        *pixels = &this->mPixels[aY * pitch + aX];

  while (aH > 3) {
    *pixels = aColor;
    pixels += pitch;
    *pixels = aColor;
    pixels += pitch;
    *pixels = aColor;
    pixels += pitch;
    *pixels = aColor;
    pixels += pitch;
    aH -= 4;
  }

  while (aH > 0) {
    *pixels = aColor;
    pixels += pitch;
    aH--;
  }
}

void BBitmap::DrawLine(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint8 aColor) {
  TInt viewPortOffsetX = 0;
  TInt viewPortOffsetY = 0;

  TUint8  *pixels;
  TUint32 pitch        = this->mPitch;

  // Create the viewport
  TRect clipRect;
  if (aViewPort) {
    aViewPort->GetRect(clipRect);
    viewPortOffsetX = TInt(round(aViewPort->mOffsetX));
    viewPortOffsetY = TInt(round(aViewPort->mOffsetY));
    aX1 += viewPortOffsetX;
    aX2 += viewPortOffsetX;
    aY1 += viewPortOffsetY;
    aY2 += viewPortOffsetY;
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
  }

  // Store viewport width/height
  const TInt clipRectWidth  = clipRect.Width();
  const TInt clipRectHeight = clipRect.Height();

  // Draw simple lines if possible
  if (aY1 == aY2) {
    // Do y bounds checks
    if (aY1 < viewPortOffsetY || aY1 >= clipRectHeight) {
      return;
    }

    if (aX1 == aX2) {
      if (aX1 >= viewPortOffsetX && aX1 < clipRectWidth) {
        // Draw a single pixel at aX1, aY1
        this->mPixels[aY1 * pitch + aX1] = aColor;
      }
    } else if (aX2 > aX1) {
      // Draw horizontal line at aX1, aY1

      // last x point + 1
      TInt xEnd = aX2 + 1;

      // Check if the entire line is not on the display
      if (xEnd <= viewPortOffsetX || aX1 >= clipRectWidth) {
        return;
      }

      // Don't start before the left edge
      if (aX1 < viewPortOffsetX) {
        aX1 = viewPortOffsetX;
      }

      // Don't end past the right edge
      if (xEnd > clipRectWidth) {
        xEnd = clipRectWidth;
      }

      // calculate actual width (even if unchanged)
      auto w = TUint(xEnd - aX1);

      pixels = &this->mPixels[aY1 * pitch + aX1];

      while (w > 3) {
        *pixels++ = aColor;
        *pixels++ = aColor;
        *pixels++ = aColor;
        *pixels++ = aColor;
        w -= 4;
      }

      while (w > 0) {
        *pixels++ = aColor;
        w--;
      }
    } else {
      // Draw horizontal line at aX2, aY1

      // last x point + 1
      TInt xEnd = aX1 + 1;

      // Check if the entire line is not on the display
      if (xEnd <= viewPortOffsetX || aX1 >= clipRectWidth) {
        return;
      }

      // Don't start before the left edge
      if (aX2 < viewPortOffsetX) {
        aX2 = viewPortOffsetX;
      }

      // Don't end past the right edge
      if (xEnd > clipRectWidth) {
        xEnd = clipRectWidth;
      }

      // calculate actual width (even if unchanged)
      auto w = TUint(xEnd - aX2);

      pixels = &this->mPixels[aY1 * pitch + aX2];

      while (w > 3) {
        *pixels++ = aColor;
        *pixels++ = aColor;
        *pixels++ = aColor;
        *pixels++ = aColor;
        w -= 4;
      }

      while (w > 0) {
        *pixels++ = aColor;
        w--;
      }
    }
    return;
  } else if (aX1 == aX2) {
    // Do x bounds checks
    if (aX1 < viewPortOffsetX || aX1 >= clipRectWidth) {
      return;
    }
    if (aY2 > aY1) {
      // Draw vertical line at aX1, aY1

      // last y point + 1
      TInt yEnd = aY2 + 1;

      // Check if the entire line is not on the display
      if (yEnd <= viewPortOffsetY || aY1 >= clipRectHeight) {
        return;
      }

      // Don't start before the top edge
      if (aY1 < viewPortOffsetY) {
        aY1 = viewPortOffsetY;
      }

      // Don't end past the bottom edge
      if (yEnd > clipRectHeight) {
        yEnd = clipRectHeight;
      }

      // calculate actual height (even if unchanged)
      auto h = TUint(yEnd - aY1);

      pixels = &this->mPixels[aY1 * pitch + aX1];
      while (h > 3) {
        *pixels = aColor;
        pixels += pitch;
        *pixels = aColor;
        pixels += pitch;
        *pixels = aColor;
        pixels += pitch;
        *pixels = aColor;
        pixels += pitch;
        h -= 4;
      }

      while (h > 0) {
        *pixels = aColor;
        pixels += pitch;
        h--;
      }
    } else {
      // Draw vertical line at aX1, aY2

      // last y point + 1
      TInt yEnd = aY1 + 1;

      // Check if the entire line is not on the display
      if (yEnd <= viewPortOffsetY || aY2 >= clipRectHeight) {
        return;
      }

      // Don't start before the top edge
      if (aY2 < viewPortOffsetY) {
        aY2 = viewPortOffsetY;
      }

      // Don't end past the bottom edge
      if (yEnd > clipRectHeight) {
        yEnd = clipRectHeight;
      }

      // calculate actual height (even if unchanged)
      auto h = TUint(yEnd - aY2);

      pixels = &this->mPixels[aY2 * pitch + aX1];
      while (h > 3) {
        *pixels = aColor;
        pixels += pitch;
        *pixels = aColor;
        pixels += pitch;
        *pixels = aColor;
        pixels += pitch;
        *pixels = aColor;
        pixels += pitch;
        h -= 4;
      }

      while (h > 0) {
        *pixels = aColor;
        pixels += pitch;
        h--;
      }
    }
    return;
  }

  // Bresenham's algorithm
  // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
  const TBool steep = ABS(aY2 - aY1) > ABS(aX2 - aX1);

  if (steep) {
    TInt temp = aX1;
    aX1 = aY1;
    aY1 = temp;

    temp = aX2;
    aX2  = aY2;
    aY2  = temp;
  }

  if (aX1 > aX2) {
    TInt temp = aX1;
    aX1 = aX2;
    aX2 = temp;

    temp = aY1;
    aY1  = aY2;
    aY2  = temp;
  }

  const TInt dx    = aX2 - aX1;
  const TInt dy    = ABS(aY2 - aY1);
  const TInt ystep = aY1 < aY2 ? 1 : -1;

  TInt err = dx / 2;

  const TFloat m = (TFloat) (aY2 - aY1) / (aX2 - aX1);

  if (steep) {
    if (aX1 < viewPortOffsetY || aX1 >= clipRectHeight) {
      aX1 = aX1 < viewPortOffsetY ? viewPortOffsetY : clipRectHeight - 1;
      const TInt tempDeltaX = MAX(1, aX2 - aX1);
      aY1 = TInt(-tempDeltaX * (m - ((TFloat) aY2 / tempDeltaX)));
    }

    if (aY1 < viewPortOffsetX || aY1 >= clipRectWidth) {
      aY1 = aY1 < viewPortOffsetX ? viewPortOffsetX : clipRectWidth - 1;
      const TInt tempDeltaY = aY2 - aY1;
      aX1 = MAX(viewPortOffsetY, TInt((-tempDeltaY + (m * aX2)) / m));
    }

    if (aX2 >= clipRectHeight || aX2 < viewPortOffsetY || aY2 >= clipRectWidth || aY2 < viewPortOffsetX) {
      aY2 = aY2 < viewPortOffsetX ? viewPortOffsetX : MIN(aY2, clipRectWidth - 1);
      aX2 = MIN(clipRectHeight - 1, TInt(((aY2
        -aY1) + (m * aX1)) / m));
    }

    // aY1 is X coord and aX1 is Y coord in this case
    for (; aX1 <= aX2; aX1++) {
      this->mPixels[aX1 * pitch + aY1] = aColor;

      err -= dy;

      if (err < 0) {
        aY1 += ystep;
        err += dx;
      }
    }
  } else {
    if (aY1 < viewPortOffsetY || aY1 >= clipRectHeight) {
      aY1 = aY1 < viewPortOffsetY ? viewPortOffsetY : clipRectHeight - 1;
      const TInt tempDeltaY = aY2 - aY1;
      aX1 = TInt((-tempDeltaY + (m * aX2)) / m);
    }

    if (aX1 < viewPortOffsetX || aX1 >= clipRectWidth) {
      aX1 = aX1 < viewPortOffsetX ? viewPortOffsetX : clipRectWidth - 1;
      const TInt tempDeltaX = MAX(1, aX2 - aX1);
      aY1 = MAX(viewPortOffsetY, TInt(-tempDeltaX * (m - ((TFloat) aY2 / tempDeltaX))));
    }

    if (aY2 >= clipRectHeight || aY2 < viewPortOffsetY || aX2 >= clipRectWidth || aX2 < viewPortOffsetX) {
      aY2 = aY2 < viewPortOffsetY ? viewPortOffsetY : MIN(aY2, clipRectHeight - 1);
      aX2 = MIN(clipRectWidth - 1, TInt(((aY2
        -aY1) + (m * aX1)) / m));
    }

    // aX1 is X coord and aY1 is Y coord in this case
    for (; aX1 <= aX2; aX1++) {
      this->mPixels[aY1 * pitch + aX1] = aColor;

      err -= dy;

      if (err < 0) {
        aY1 += ystep;
        err += dx;
      }
    }
  }
}

void BBitmap::DrawRect(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint8 aColor) {
  TInt x2, y2, w;
  TInt viewPortOffsetX = 0;
  TInt viewPortOffsetY = 0;

  TUint8        *pixels;
  const TUint32 pitch  = this->mPitch;

  // Create the viewport
  TRect clipRect;
  if (aViewPort) {
    aViewPort->GetRect(clipRect);
    viewPortOffsetX = TInt(round(aViewPort->mOffsetX));
    viewPortOffsetY = TInt(round(aViewPort->mOffsetY));
    aX1 += viewPortOffsetX;
    aX2 += viewPortOffsetX;
    aY1 += viewPortOffsetY;
    aY2 += viewPortOffsetY;
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
  }

  // Store viewport width/height
  const TInt clipRectWidth  = clipRect.Width();
  const TInt clipRectHeight = clipRect.Height();

  // calculate boundaries
  TInt xMax  = MAX(viewPortOffsetX, aX1);
  TInt yDest = MAX(0, aY1) * pitch;
  aX2 = MIN(clipRectWidth, aX2);
  w   = aX2 - xMax;

  // Draw horizontal lines
  if (aX2 >= viewPortOffsetX && aX1 < clipRectWidth) {
    // Draw rectangle's top side
    if (aY1 >= viewPortOffsetY && aY1 < clipRectHeight) {
      // cache initial coordinates
      x2 = w;

      pixels = &this->mPixels[yDest + xMax];
      while (x2 > 3) {
        *pixels++ = aColor;
        *pixels++ = aColor;
        *pixels++ = aColor;
        *pixels++ = aColor;
        x2 -= 4;
      }

      while (x2 > 0) {
        *pixels++ = aColor;
        x2--;
      }
    }

    // Draw rectangle's bottom side
    if (aY2 >= viewPortOffsetY && aY2 < clipRectHeight) {
      // cache initial coordinates
      x2 = w;

      pixels = &this->mPixels[aY2 * pitch + xMax];
      while (x2 > 3) {
        *pixels++ = aColor;
        *pixels++ = aColor;
        *pixels++ = aColor;
        *pixels++ = aColor;
        x2 -= 4;
      }

      while (x2 > 0) {
        *pixels++ = aColor;
        x2--;
      }
    }
  }

  // Draw vertical lines
  if (aY2 >= viewPortOffsetY && aY1 < clipRectHeight) {
    // Don't start before the top edge
    if (aY1 < viewPortOffsetY) {
      aY1   = viewPortOffsetY;
      yDest = aY1 * pitch;
    }

    // Don't end past the bottom edge
    if (aY2 > clipRectHeight) {
      aY2 = clipRectHeight;
    }

    // calculate actual height (even if unchanged)
    aY2 -= aY1 - 1;

    // Draw rectangle's left side
    if (aX1 >= viewPortOffsetX && aX1 < clipRectWidth) {
      // cache initial coordinates
      y2 = aY2;

      pixels = &this->mPixels[yDest + aX1];
      while (y2 > 3) {
        *pixels = aColor;
        pixels += pitch;
        *pixels = aColor;
        pixels += pitch;
        *pixels = aColor;
        pixels += pitch;
        *pixels = aColor;
        pixels += pitch;
        y2 -= 4;
      }

      while (y2 > 0) {
        *pixels = aColor;
        pixels += pitch;
        y2--;
      }
    }

    // Draw rectangle's right side
    if (aX2 >= viewPortOffsetX && aX2 < clipRectWidth) {
      // cache initial coordinates
      y2 = aY2;

      pixels = &this->mPixels[yDest + aX2];
      while (y2 > 3) {
        *pixels = aColor;
        pixels += pitch;
        *pixels = aColor;
        pixels += pitch;
        *pixels = aColor;
        pixels += pitch;
        *pixels = aColor;
        pixels += pitch;
        y2 -= 4;
      }

      while (y2 > 0) {
        *pixels = aColor;
        pixels += pitch;
        y2--;
      }
    }
  }
}

void BBitmap::FillRect(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint8 aColor) {
  TInt viewPortOffsetX = 0;
  TInt viewPortOffsetY = 0;

  // Create the viewport
  TRect clipRect;
  if (aViewPort) {
    aViewPort->GetRect(clipRect);
    viewPortOffsetX = TInt(round(aViewPort->mOffsetX));
    viewPortOffsetY = TInt(round(aViewPort->mOffsetY));
    aX1 += viewPortOffsetX;
    aX2 += viewPortOffsetX;
    aY1 += viewPortOffsetY;
    aY2 += viewPortOffsetY;
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
  }

  // Store viewport width/height
  const TInt clipRectWidth  = clipRect.Width();
  const TInt clipRectHeight = clipRect.Height();

  // Check if the entire rect is not on the display
  if (aX2 <= viewPortOffsetX || aX1 >= clipRectWidth || aY2 <= viewPortOffsetY || aY1 >= clipRectHeight) {
    return;
  }

  // Don't start before the left edge
  if (aX1 < viewPortOffsetX) {
    aX1 = viewPortOffsetX;
  }

  // Don't start before the top edge
  if (aY1 < viewPortOffsetY) {
    aY1 = viewPortOffsetY;
  }

  // Don't end past the right edge
  if (aX2 > clipRectWidth) {
    aX2 = clipRectWidth;
  }

  // Don't end past the bottom edge
  if (aY2 > clipRectHeight) {
    aY2 = clipRectHeight;
  }

  // calculate actual width and height (even if unchanged)
  aX2 -= aX1 - 1;
  aY2 -= aY1 - 1;

  TUint8        *pixels;
  const TUint32 pitch       = this->mPitch;
  const TUint32 yDest       = aY1 * pitch;

  while (aX2--) {
    TInt h = aY2;
    pixels = &this->mPixels[yDest + aX1++];

    while (h > 3) {
      *pixels = aColor;
      pixels += pitch;
      *pixels = aColor;
      pixels += pitch;
      *pixels = aColor;
      pixels += pitch;
      *pixels = aColor;
      pixels += pitch;
      h -= 4;
    }

    while (h--) {
      *pixels = aColor;
      pixels += pitch;
    }
  }
}

void BBitmap::DrawCircle(BViewPort *aViewPort, TInt aX, TInt aY, TUint r, TUint8 aColor) {
  TInt viewPortOffsetX = 0;
  TInt viewPortOffsetY = 0;

  // Create the viewport
  TRect clipRect;
  if (aViewPort) {
    aViewPort->GetRect(clipRect);
    viewPortOffsetX = TInt(round(aViewPort->mOffsetX));
    viewPortOffsetY = TInt(round(aViewPort->mOffsetY));
    aX += viewPortOffsetX;
    aY += viewPortOffsetY;
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
  }

  // Store viewport width/height
  const TInt clipRectWidth  = clipRect.Width();
  const TInt clipRectHeight = clipRect.Height();

  TInt maxX = aX + r;
  TInt minX = aX - r;
  TInt maxY = aY + r;
  TInt minY = aY - r;

  // Circle is outside of the viewport
  if (maxX < viewPortOffsetX || minX > clipRectWidth || maxY < viewPortOffsetY || minY > clipRectHeight) {
    return;
  }

  TInt          f     = 1 - r;
  TInt          ddF_x = 1;
  TInt          ddF_y = -(r << 1);
  TInt          x     = 0;
  TInt          y     = r;
  TInt          xx1, xx2, xx3, xx4, yy1, yy2, yy3, yy4;
  TUint32       yy1Dest, yy2Dest, yy3Dest, yy4Dest;
  const TUint32 pitch = this->mPitch;

  // Circle is inside the viewport
  if (minX >= viewPortOffsetX && maxX < clipRectWidth && minY >= viewPortOffsetY && maxY < clipRectHeight) {
    // top and bottom center pixels
    this->mPixels[maxY * pitch + aX] = aColor;
    this->mPixels[minY * pitch + aX] = aColor;

    // left and right center pixels
    this->mPixels[aY * pitch + maxX] = aColor;
    this->mPixels[aY * pitch + minX] = aColor;

    while (x < y) {
      if (f >= 0) {
        y--;
        ddF_y += 2;
        f += ddF_y;
      }

      x++;
      ddF_x += 2;
      f += ddF_x;

      xx1     = aX + x;
      xx2     = aX - x;
      xx3     = aX + y;
      xx4     = aX - y;
      yy1     = aY + y;
      yy2     = aY - y;
      yy3     = aY + x;
      yy4     = aY - x;
      yy1Dest = yy1 * pitch;
      yy2Dest = yy2 * pitch;
      yy3Dest = yy3 * pitch;
      yy4Dest = yy4 * pitch;

      // top and bottom right arcs
      this->mPixels[yy1Dest + xx1] = aColor;
      this->mPixels[yy2Dest + xx1] = aColor;
      this->mPixels[yy3Dest + xx3] = aColor;
      this->mPixels[yy4Dest + xx3] = aColor;

      // top and bottom left arcs
      this->mPixels[yy1Dest + xx2] = aColor;
      this->mPixels[yy2Dest + xx2] = aColor;
      this->mPixels[yy3Dest + xx4] = aColor;
      this->mPixels[yy4Dest + xx4] = aColor;
    }
  } else {
    // Circle is partially inside the viewport
    TBool yy1Visible, yy2Visible, yy3Visible, yy4Visible;

    // top and bottom center pixels
    if (aX >= viewPortOffsetX && aX < clipRectWidth) {
      if (maxY < clipRectHeight) {
        this->mPixels[maxY * pitch + aX] = aColor;
      }
      if (minY >= viewPortOffsetY) {
        this->mPixels[minY * pitch + aX] = aColor;
      }
    }
    // left and right center pixels
    if (aY >= viewPortOffsetY && aY < clipRectHeight) {
      if (maxX < clipRectWidth) {
        this->mPixels[aY * pitch + maxX] = aColor;
      }
      if (minX >= viewPortOffsetX) {
        this->mPixels[aY * pitch + minX] = aColor;
      }
    }

    while (x < y) {
      if (f >= 0) {
        y--;
        ddF_y += 2;
        f += ddF_y;
      }

      x++;
      ddF_x += 2;
      f += ddF_x;

      xx1        = aX + x;
      xx2        = aX - x;
      xx3        = aX + y;
      xx4        = aX - y;
      yy1        = aY + y;
      yy2        = aY - y;
      yy3        = aY + x;
      yy4        = aY - x;
      yy1Dest    = yy1 * pitch;
      yy2Dest    = yy2 * pitch;
      yy3Dest    = yy3 * pitch;
      yy4Dest    = yy4 * pitch;
      yy1Visible = yy1 >= viewPortOffsetY && yy1 < clipRectHeight;
      yy2Visible = yy2 >= viewPortOffsetY && yy2 < clipRectHeight;
      yy3Visible = yy3 >= viewPortOffsetY && yy3 < clipRectHeight;
      yy4Visible = yy4 >= viewPortOffsetY && yy4 < clipRectHeight;

      // top and bottom right arcs
      if (xx1 >= viewPortOffsetX && xx1 < clipRectWidth) {
        if (yy1Visible) {
          this->mPixels[yy1Dest + xx1] = aColor;
        }
        if (yy2Visible) {
          this->mPixels[yy2Dest + xx1] = aColor;
        }
      }
      if (xx3 >= viewPortOffsetX && xx3 < clipRectWidth) {
        if (yy3Visible) {
          this->mPixels[yy3Dest + xx3] = aColor;
        }
        if (yy4Visible) {
          this->mPixels[yy4Dest + xx3] = aColor;
        }
      }

      // top and bottom left arcs
      if (xx2 >= viewPortOffsetX && xx2 < clipRectWidth) {
        if (yy1Visible) {
          this->mPixels[yy1Dest + xx2] = aColor;
        }
        if (yy2Visible) {
          this->mPixels[yy2Dest + xx2] = aColor;
        }
      }
      if (xx4 >= viewPortOffsetX && xx4 < clipRectWidth) {
        if (yy3Visible) {
          this->mPixels[yy3Dest + xx4] = aColor;
        }
        if (yy4Visible) {
          this->mPixels[yy4Dest + xx4] = aColor;
        }
      }
    }
  }
}

void BBitmap::FillCircle(BViewPort *aViewPort, TInt aX, TInt aY, TUint r, TUint8 aColor) {
  TInt viewPortOffsetX = 0;
  TInt viewPortOffsetY = 0;

  // Create the viewport
  TRect clipRect;
  if (aViewPort) {
    aViewPort->GetRect(clipRect);
    viewPortOffsetX = TInt(round(aViewPort->mOffsetX));
    viewPortOffsetY = TInt(round(aViewPort->mOffsetY));
    aX += viewPortOffsetX;
    aY += viewPortOffsetY;
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
  }

  // Store viewport width/height
  const TInt clipRectWidth  = clipRect.Width();
  const TInt clipRectHeight = clipRect.Height();

  TInt maxX = aX + r;
  TInt minX = aX - r;
  TInt maxY = aY + r;
  TInt minY = aY - r;

  // Circle is outside of the viewport
  if (maxX < viewPortOffsetX || minX > clipRectWidth || maxY < viewPortOffsetY || minY > clipRectHeight) {
    return;
  }

  TInt  f     = 1 - r;
  TInt  ddF_x = 1;
  TInt  ddF_y = -(r << 1);
  TUint x     = 0;
  TUint y     = r;
  TUint w     = (r << 1) + 1;
  TInt  xx1, yy1, yy2;

  const TUint32 pitch = this->mPitch;

  // Circle is inside the viewport
  if (minX >= viewPortOffsetX && maxX < clipRectWidth && minY >= viewPortOffsetY && maxY < clipRectHeight) {
    TUint8 *pixels1 = &this->mPixels[aY * pitch + aX - r];
    TUint8 *pixels2;

    // Central line
    while (w > 3) {
      *pixels1++ = aColor;
      *pixels1++ = aColor;
      *pixels1++ = aColor;
      *pixels1++ = aColor;
      w -= 4;
    }

    while (w > 0) {
      *pixels1++ = aColor;
      w--;
    }

    // Left and right sides
    while (x < y) {
      if (f >= 0) {
        y--;
        ddF_y += 2;
        f += ddF_y;
      }

      x++;
      ddF_x += 2;
      f += ddF_x;

      // Top and bottom parts
      w       = (x << 1) + 1;
      xx1     = aX - x;
      yy1     = aY + y;
      yy2     = aY - y;
      pixels1 = &this->mPixels[yy1 * pitch + xx1];
      pixels2 = &this->mPixels[yy2 * pitch + xx1];
      while (w > 3) {
        *pixels1++ = aColor;
        *pixels1++ = aColor;
        *pixels1++ = aColor;
        *pixels1++ = aColor;
        *pixels2++ = aColor;
        *pixels2++ = aColor;
        *pixels2++ = aColor;
        *pixels2++ = aColor;
        w -= 4;
      }

      while (w > 0) {
        *pixels1++ = aColor;
        *pixels2++ = aColor;
        w--;
      }

      // Center parts
      w       = (y << 1) + 1;
      xx1     = aX - y;
      yy1     = aY + x;
      yy2     = aY - x;
      pixels1 = &this->mPixels[yy1 * pitch + xx1];
      pixels2 = &this->mPixels[yy2 * pitch + xx1];
      while (w > 3) {
        *pixels1++ = aColor;
        *pixels1++ = aColor;
        *pixels1++ = aColor;
        *pixels1++ = aColor;
        *pixels2++ = aColor;
        *pixels2++ = aColor;
        *pixels2++ = aColor;
        *pixels2++ = aColor;
        w -= 4;
      }

      while (w > 0) {
        *pixels1++ = aColor;
        *pixels2++ = aColor;
        w--;
      }
    }
  } else {
    TUint8 *pixels;

    // Circle is partially inside the viewport
    xx1 = aX - r;

    // last x point + 1
    TInt xEnd = xx1 + w;

    // Central line
    if (aY >= viewPortOffsetY && aY < clipRectHeight && xEnd >= viewPortOffsetX && xx1 < clipRectWidth) {
      // Don't start before the left edge
      if (xx1 < viewPortOffsetX) {
        xx1 = viewPortOffsetX;
      }

      // Don't end past the right edge
      if (xEnd > clipRectWidth) {
        xEnd = clipRectWidth;
      }

      // calculate actual width (even if unchanged)
      w      = TUint(xEnd - xx1);
      pixels = &this->mPixels[aY * pitch + xx1];

      while (w > 3) {
        *pixels++ = aColor;
        *pixels++ = aColor;
        *pixels++ = aColor;
        *pixels++ = aColor;
        w -= 4;
      }

      while (w > 0) {
        *pixels++ = aColor;
        w--;
      }
    }

    // Top and bottom sides
    while (x < y) {
      if (f >= 0) {
        y--;
        ddF_y += 2;
        f += ddF_y;
      }

      x++;
      ddF_x += 2;
      f += ddF_x;

      // Top and bottom parts
      w    = (x << 1) + 1;
      xx1  = aX - x;
      xEnd = xx1 + w;
      yy1  = aY + y;
      yy2  = aY - y;

      if (xEnd >= viewPortOffsetX && xx1 < clipRectWidth) {
        // Don't start before the left edge
        if (xx1 < viewPortOffsetX) {
          xx1 = viewPortOffsetX;
        }

        // Don't end past the right edge
        if (xEnd > clipRectWidth) {
          xEnd = clipRectWidth;
        }

        // calculate actual width (even if unchanged)
        w = TUint(xEnd - xx1);

        // Bottom part
        if (yy1 >= viewPortOffsetY && yy1 < clipRectHeight) {
          pixels = &this->mPixels[yy1 * pitch + xx1];
          while (w > 3) {
            *pixels++ = aColor;
            *pixels++ = aColor;
            *pixels++ = aColor;
            *pixels++ = aColor;
            w -= 4;
          }

          while (w > 0) {
            *pixels++ = aColor;
            w--;
          }
        }

        // Top part
        if (yy2 >= viewPortOffsetY && yy2 < clipRectHeight) {
          w      = TUint(xEnd - xx1);
          pixels = &this->mPixels[yy2 * pitch + xx1];
          while (w > 3) {
            *pixels++ = aColor;
            *pixels++ = aColor;
            *pixels++ = aColor;
            *pixels++ = aColor;
            w -= 4;
          }

          while (w > 0) {
            *pixels++ = aColor;
            w--;
          }
        }
      }

      // Center parts
      w    = (y << 1) + 1;
      xx1  = aX - y;
      xEnd = xx1 + w;
      yy1  = aY + x;
      yy2  = aY - x;
      if (xEnd >= viewPortOffsetX && xx1 < clipRectWidth) {
        // Don't start before the left edge
        if (xx1 < viewPortOffsetX) {
          xx1 = viewPortOffsetX;
        }

        // Don't end past the right edge
        if (xEnd > clipRectWidth) {
          xEnd = clipRectWidth;
        }

        // calculate actual width (even if unchanged)
        w = TUint(xEnd - xx1);

        // Bottom part
        if (yy1 >= viewPortOffsetY && yy1 < clipRectHeight) {
          pixels = &this->mPixels[yy1 * pitch + xx1];
          while (w > 3) {
            *pixels++ = aColor;
            *pixels++ = aColor;
            *pixels++ = aColor;
            *pixels++ = aColor;
            w -= 4;
          }

          while (w > 0) {
            *pixels++ = aColor;
            w--;
          }
        }

        // Top part
        if (yy2 >= viewPortOffsetY && yy2 < clipRectHeight) {
          w      = TUint(xEnd - xx1);
          pixels = &this->mPixels[yy2 * pitch + xx1];
          while (w > 3) {
            *pixels++ = aColor;
            *pixels++ = aColor;
            *pixels++ = aColor;
            *pixels++ = aColor;
            w -= 4;
          }

          while (w > 0) {
            *pixels++ = aColor;
            w--;
          }
        }
      }
    }
  }
}
