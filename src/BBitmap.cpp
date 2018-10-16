#include "BBitmap.h"
#include <string.h>
#include "Panic.h"
#include "BFont.h"
#include <strings.h>

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
  const TInt deltaImageWidth  = aX < 0 ? -aSrcRect.Width() + w : aSrcRect.Width() - w;
  const TInt deltaImageHeight = aY < 0 ? -aSrcRect.Height() + h : aSrcRect.Height() - h;

  // Calculate visible width and height to iterate over
  const TInt i = -sy + h;
  const TInt j = -sx + w;

  if (aFlipped) {
    if (aFlopped) {
      if (aLeft && !aRight) {
        // flip and flop and rotate left
        for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
          for (TInt xx = -sx, dxx = dx, rsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, rsx--) {
            // Read pixel value from bitmap
            TUint8 pix = aSrcBitmap->ReadPixel(yy, rsx);

            // Write pixel values
            WritePixel(dxx, dyy, pix);
          }
        }
      } else if (aRight && !aLeft) {
        // flip and flop and rotate right
        for (TInt yy = -sy, dyy = dy, rsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, rsy--) {
          for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
            // Read pixel value from bitmap
            TUint8 pix = aSrcBitmap->ReadPixel(rsy, xx);

            // Write pixel values
            WritePixel(dxx, dyy, pix);
          }
        }
      } else {
        // flipped and flopped
        for (TInt yy = -sy, dyy = dy, fsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, fsy--) {
          for (TInt xx = -sx, dxx = dx, fsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, fsx--) {
            // Read pixel value from bitmap
            TUint8 pix = aSrcBitmap->ReadPixel(fsx, fsy);

            // Write pixel values
            WritePixel(dxx, dyy, pix);
          }
        }
      }
    } else {
      if (aLeft && !aRight) {
        // flip and rotate left
        for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
          for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
            // Read pixel value from bitmap
            TUint8 pix = aSrcBitmap->ReadPixel(yy, xx);

            // Write pixel values
            WritePixel(dxx, dyy, pix);
          }
        }
      } else if (aRight && !aLeft) {
        // flip and rotate right
        for (TInt yy = -sy, dyy = dy, rsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, rsy--) {
          for (TInt xx = -sx, dxx = dx, fsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, fsx--) {
            // Read pixel value from bitmap
            TUint8 pix = aSrcBitmap->ReadPixel(rsy, fsx);

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
    }
  } else if (aFlopped) {
    if (aLeft && !aRight) {
      // flop and rotate left
      for (TInt yy = -sy, dyy = dy, fsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, fsy--) {
        for (TInt xx = -sx, dxx = dx, rsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, rsx--) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(fsy, rsx);

          // Write pixel values
          WritePixel(dxx, dyy, pix);
        }
      }
    } else if (aRight && !aLeft) {
      // flop and rotate right
      for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
        for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(yy, xx);

          // Write pixel values
          WritePixel(dxx, dyy, pix);
        }
      }
    } else {
      // flopped
      for (TInt yy = -sy, dyy = dy, fsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, fsy--) {
        for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(xx, fsy);

          // Write pixel values
          WritePixel(dxx, dyy, pix);
        }
      }
    }
  } else {
    if (aLeft && !aRight) {
      // rotate left
      for (TInt yy = -sy, dyy = dy, rsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, rsy--) {
        for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(rsy, xx);

          // Write pixel values
          WritePixel(dxx, dyy, pix);
        }
      }
    } else if (aRight && !aLeft) {
      // rotate right
      for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
        for (TInt xx = -sx, dxx = dx, rsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, rsx--) {
          // Read pixel value from bitmap
          TUint8 pix = aSrcBitmap->ReadPixel(yy, rsx);

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
  }

  return ETrue;
}

void BBitmap::CopyPixels(BBitmap *aOther) {
  if (aOther->mWidth != mWidth || aOther->mHeight != mHeight) {
    printf("CopyPixels: other bitmap has different dimensions");
    return;
  }
  memcpy(mPixels, aOther->mPixels, mWidth*mHeight);
}

TBool BBitmap::DrawSprite(BViewPort *aViewPort, TInt16 aBitmapSlot, TInt aImageNumber,
                          TInt aX, TInt aY, TUint32 aFlags) {
  BBitmap *b              = gResourceManager.GetBitmap(aBitmapSlot);
  TInt    bw              = gResourceManager.BitmapWidth(aBitmapSlot),
          bh              = gResourceManager.BitmapHeight(aBitmapSlot),
          pitch           = b->mWidth / bw,
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
  const TInt deltaImageWidth  = (aX < 0 ? -imageRect.Width() + w : imageRect.Width() - w) - 1;
  const TInt deltaImageHeight = (aY < 0 ? -imageRect.Height() + h : imageRect.Height() - h) - 1;

  // Calculate visible width and height to iterate over
  const TInt i = -sy + h;
  const TInt j = -sx + w;

  if (aFlags & SFLAG_FLIP) {
    if (aFlags & SFLAG_FLOP) {
      if (aFlags & SFLAG_LEFT && !(aFlags & SFLAG_RIGHT)) {
        // flip and flop and rotate left
        for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
          for (TInt xx = -sx, dxx = dx, rsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, rsx--) {
            // Read pixel value from bitmap
            TUint8 pix = b->ReadPixel(yy, rsx);

            // Write non-transparent pixel values
            if (pix != t) {
              WritePixel(dxx, dyy, pix);
            }
          }
        }
      } else if (aFlags & SFLAG_RIGHT && !(aFlags & SFLAG_LEFT)) {
        // flip and flop and rotate right
        for (TInt yy = -sy, dyy = dy, rsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, rsy--) {
          for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
            // Read pixel value from bitmap
            TUint8 pix = b->ReadPixel(rsy, xx);

            // Write non-transparent pixel values
            if (pix != t) {
              WritePixel(dxx, dyy, pix);
            }
          }
        }
      } else {
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
      }
    } else {
      if (aFlags & SFLAG_LEFT && !(aFlags & SFLAG_RIGHT)) {
        // flip and rotate left
        for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
          for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
            // Read pixel value from bitmap
            TUint8 pix = b->ReadPixel(yy, xx);

            // Write non-transparent pixel values
            if (pix != t) {
              WritePixel(dxx, dyy, pix);
            }
          }
        }
      } else if (aFlags & SFLAG_RIGHT && !(aFlags & SFLAG_LEFT)) {
        // flip and rotate right
        for (TInt yy = -sy, dyy = dy, rsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, rsy--) {
          for (TInt xx = -sx, dxx = dx, fsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, fsx--) {
            // Read pixel value from bitmap
            TUint8 pix = b->ReadPixel(rsy, fsx);

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
    }
  } else if (aFlags & SFLAG_FLOP) {
      if (aFlags & SFLAG_LEFT && !(aFlags & SFLAG_RIGHT)) {
        // flop and rotate left
        for (TInt yy = -sy, dyy = dy, fsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, fsy--) {
          for (TInt xx = -sx, dxx = dx, rsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, rsx--) {
            // Read pixel value from bitmap
            TUint8 pix = b->ReadPixel(fsy, rsx);

            // Write non-transparent pixel values
            if (pix != t) {
              WritePixel(dxx, dyy, pix);
            }
          }
        }
      } else if (aFlags & SFLAG_RIGHT && !(aFlags & SFLAG_LEFT)) {
        // flop and rotate right
        for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
          for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
            // Read pixel value from bitmap
            TUint8 pix = b->ReadPixel(yy, xx);

            // Write non-transparent pixel values
            if (pix != t) {
              WritePixel(dxx, dyy, pix);
            }
          }
        }
      } else {
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
      }
  } else {
    if (aFlags & SFLAG_LEFT && !(aFlags & SFLAG_RIGHT)) {
      // rotate left
      for (TInt yy = -sy, dyy = dy, rsy = h + yy + deltaImageHeight; yy < i; yy++, dyy++, rsy--) {
        for (TInt xx = -sx, dxx = dx; xx < j; xx++, dxx++) {
          // Read pixel value from bitmap
          TUint8 pix = b->ReadPixel(rsy, xx);

          // Write non-transparent pixel values
          if (pix != t) {
            WritePixel(dxx, dyy, pix);
          }
        }
      }
    } else if (aFlags & SFLAG_RIGHT && !(aFlags & SFLAG_LEFT)) {
      // rotate right
      for (TInt yy = -sy, dyy = dy; yy < i; yy++, dyy++) {
        for (TInt xx = -sx, dxx = dx, rsx = w + xx + deltaImageWidth; xx < j; xx++, dxx++, rsx--) {
          // Read pixel value from bitmap
          TUint8 pix = b->ReadPixel(yy, rsx);

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
  }

  return ETrue;
}

TBool BBitmap::DrawString(BViewPort *aViewPort, BFont *aFont, TInt aDstX, TInt aDstY, const char *aString) {
  TBool drawn = false;
  while (*aString) {
    const char c = *aString++;
    drawn |= DrawSprite(aViewPort, aFont->mBitmapSlot, (TInt) c, aDstX, aDstY);
    aDstX += 8;
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
    viewPortOffsetX = aViewPort->mOffsetX;
    viewPortOffsetY = aViewPort->mOffsetY;
    aX += viewPortOffsetX;
    aY += viewPortOffsetY;
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  }

  // Store viewport width/height
  const TInt clipRectWidth = clipRect.Width();
  const TInt clipRectHeight = clipRect.Height();

  // Do y bounds checks
  if (aY < viewPortOffsetY || aY >= clipRectHeight) {
    return;
  }

  // last x point + 1
  TInt16 xEnd = aX + aW;

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
  aW = xEnd - aX;

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
    viewPortOffsetX = aViewPort->mOffsetX;
    viewPortOffsetY = aViewPort->mOffsetY;
    aX += viewPortOffsetX;
    aY += viewPortOffsetY;
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  }

  // Store viewport width/height
  const TInt clipRectWidth = clipRect.Width();
  const TInt clipRectHeight = clipRect.Height();

  // Do x bounds checks
  if (aX < viewPortOffsetX || aX >= clipRectWidth) {
    return;
  }

  // last y point + 1
  TInt16 yEnd = aY + aH;

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
  aH = yEnd - aY;

  const TUint32 pitch = this->mPitch;
  TUint8 *pixels = &this->mPixels[aY * pitch + aX];

  while (aH > 3) {
    *pixels = aColor; pixels += pitch;
    *pixels = aColor; pixels += pitch;
    *pixels = aColor; pixels += pitch;
    *pixels = aColor; pixels += pitch;
    aH -= 4;
  }

  while (aH > 0) {
    *pixels = aColor; pixels += pitch;
    aH--;
  }
}

void BBitmap::DrawLine(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint8 aColor) {
  TInt viewPortOffsetX = 0;
  TInt viewPortOffsetY = 0;

  TUint8 *pixels;
  TUint32 pitch = this->mPitch;

  // Create the viewport
  TRect clipRect;
  if (aViewPort) {
    aViewPort->GetRect(clipRect);
    viewPortOffsetX = aViewPort->mOffsetX;
    viewPortOffsetY = aViewPort->mOffsetY;
    aX1 += viewPortOffsetX;
    aX2 += viewPortOffsetX;
    aY1 += viewPortOffsetY;
    aY2 += viewPortOffsetY;
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  }

  // Store viewport width/height
  const TInt clipRectWidth = clipRect.Width();
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
      TInt16 xEnd = aX1 + aX2 - aX1 + 1;

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
      TUint w = xEnd - aX1;

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
      TInt16 xEnd = aX2 + aX1 - aX2 + 1;

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
      TUint w = xEnd - aX2;

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
      TInt16 yEnd = aY1 + aY2 -aY1 + 1;

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
      TUint h = yEnd - aY1;

      pixels = &this->mPixels[aY1 * pitch + aX1];
      while (h > 3) {
        *pixels = aColor; pixels += pitch;
        *pixels = aColor; pixels += pitch;
        *pixels = aColor; pixels += pitch;
        *pixels = aColor; pixels += pitch;
        h -= 4;
      }

      while (h > 0) {
        *pixels = aColor; pixels += pitch;
        h--;
      }
    } else {
      // Draw vertical line at aX1, aY2

      // last y point + 1
      TInt16 yEnd = aY2 + aY1 - aY2 + 1;

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
      TUint h = yEnd - aY2;

      pixels = &this->mPixels[aY1 * pitch + aX1];
      while (h > 3) {
        *pixels = aColor; pixels += pitch;
        *pixels = aColor; pixels += pitch;
        *pixels = aColor; pixels += pitch;
        *pixels = aColor; pixels += pitch;
        h -= 4;
      }

      while (h > 0) {
        *pixels = aColor; pixels += pitch;
        h--;
      }
    }
    return;
  }

  // Bresenham's algorithm
  // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
  const TBool steep = ABS(aY2 - aY1) > ABS(aX2 - aX1);

  if (steep) {
    TUint temp = aX1;
    aX1 = aY1;
    aY1 = temp;

    temp = aX2;
    aX2 = aY2;
    aY2 = temp;
  }

  if (aX1 > aX2) {
    TUint temp = aX1;
    aX1 = aX2;
    aX2 = temp;

    temp = aY1;
    aY1 = aY2;
    aY2 = temp;
  }

  const TInt16 dx = aX2 - aX1;
  const TInt16 dy = ABS(aY2 - aY1);
  const TInt8 ystep = aY1 < aY2 ? 1 : -1;

  TInt16 err = dx / 2;

  const TFloat m = (TFloat) (aY2 - aY1) / (aX2 - aX1);

  if (steep) {
    if (aX1 < viewPortOffsetY || aX1 >= clipRectHeight) {
      aX1 = aX1 < viewPortOffsetY ? viewPortOffsetY : clipRectHeight;
      const TInt tempDeltaX = MAX(1, aX2 - aX1);
      aY1 = -tempDeltaX * (m - ((TFloat) aY2 / tempDeltaX));
    }

    if (aY1 < viewPortOffsetX || aY1 >= clipRectWidth) {
      aY1 = aY1 < viewPortOffsetX ? viewPortOffsetX : clipRectWidth;
      const TInt tempDeltaY = aY2 - aY1;
      aX1 = MAX(viewPortOffsetY, (-tempDeltaY + (m * aX2)) / m);
    }

    if (aX2 >= clipRectHeight || aX2 < viewPortOffsetY || aY2 >= clipRectWidth || aY2 < viewPortOffsetX) {
      aY2 = aY2 < viewPortOffsetX ? viewPortOffsetX : MIN(aY2, clipRectWidth);
      aX2 = MIN(clipRectHeight, ((aY2 - aY1) + (m * aX1)) / m);
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
      aY1 = aY1 < viewPortOffsetY ? viewPortOffsetY : clipRectHeight;
      const TInt tempDeltaY = aY2 - aY1;
      aX1 = (-tempDeltaY + (m * aX2)) / m;
    }

    if (aX1 < viewPortOffsetX || aX1 >= clipRectWidth) {
      aX1 = aX1 < viewPortOffsetX ? viewPortOffsetX : clipRectWidth;
      const TInt tempDeltaX = MAX(1, aX2 - aX1);
      aY1 = MAX(viewPortOffsetY, -tempDeltaX * (m - ((TFloat) aY2 / tempDeltaX)));
    }

    if (aY2 >= clipRectHeight || aY2 < viewPortOffsetY || aX2 >= clipRectWidth || aX2 < viewPortOffsetX) {
      aY2 = aY2 < viewPortOffsetY ? viewPortOffsetY : MIN(aY2, clipRectHeight);
      aX2 = MIN(clipRectWidth, ((aY2 - aY1) + (m * aX1)) / m);
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

  TUint8 *pixels;
  const TUint32 pitch = this->mPitch;

  // Create the viewport
  TRect clipRect;
  if (aViewPort) {
    aViewPort->GetRect(clipRect);
    viewPortOffsetX = aViewPort->mOffsetX;
    viewPortOffsetY = aViewPort->mOffsetY;
    aX1 += viewPortOffsetX;
    aY1 += viewPortOffsetY;
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  }

  // Store viewport width/height
  const TInt clipRectWidth = clipRect.Width();
  const TInt clipRectHeight = clipRect.Height();

  // calculate boundaries
  TInt16 yEnd = aY1 + aY2, xEnd = MIN(clipRectWidth, aX1 + aX2);
  TInt xMax = MAX(viewPortOffsetX, aX1);
  w = xEnd - xMax;

  // Draw horizontal lines
  if (xEnd >= viewPortOffsetX && aX1 < clipRectWidth) {
    // Draw rectangle's top side
    if (aY1 >= viewPortOffsetY && aY1 < clipRectHeight) {
      // cache initial coordinates
      x2 = w;

      pixels = &this->mPixels[aY1 * pitch + xMax];
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
    const TInt rectHeight = aY1 + aY2 - 1;
    if (rectHeight >= viewPortOffsetY && rectHeight < clipRectHeight) {
      // cache initial coordinates
      x2 = w;

      pixels = &this->mPixels[rectHeight * pitch + xMax];
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
  if (yEnd >= viewPortOffsetY && aY1 < clipRectHeight) {
    // Don't start before the top edge
    if (aY1 < viewPortOffsetY) {
      aY1 = viewPortOffsetY;
    }

    // Don't end past the bottom edge
    if (yEnd > clipRectHeight) {
      yEnd = clipRectHeight;
    }

    // calculate actual height (even if unchanged)
    aY2 = yEnd - aY1;

    // Draw rectangle's left side
    if (aX1 >= viewPortOffsetX && aX1 < clipRectWidth) {
      // cache initial coordinates
      y2 = aY2;

      pixels = &this->mPixels[aY1 * pitch + aX1];
      while (y2 > 3) {
        *pixels = aColor; pixels += pitch;
        *pixels = aColor; pixels += pitch;
        *pixels = aColor; pixels += pitch;
        *pixels = aColor; pixels += pitch;
        y2 -= 4;
      }

      while (y2 > 0) {
        *pixels = aColor; pixels += pitch;
        y2--;
      }
    }

    // Draw rectangle's right side
    const TInt rectWidth = xMax + w - 1;
    if (rectWidth >= viewPortOffsetX && rectWidth < clipRectWidth - 1) {
      // cache initial coordinates
      y2 = aY2;

      pixels = &this->mPixels[aY1 * pitch + rectWidth];
      while (y2 > 3) {
        *pixels = aColor; pixels += pitch;
        *pixels = aColor; pixels += pitch;
        *pixels = aColor; pixels += pitch;
        *pixels = aColor; pixels += pitch;
        y2 -= 4;
      }

      while (y2 > 0) {
        *pixels = aColor; pixels += pitch;
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
    viewPortOffsetX = aViewPort->mOffsetX;
    viewPortOffsetY = aViewPort->mOffsetY;
    aX1 += viewPortOffsetX;
    aY1 += viewPortOffsetY;
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  }

  // Store viewport width/height
  const TInt clipRectWidth = clipRect.Width();
  const TInt clipRectHeight = clipRect.Height();

  // last x, y points + 1
  TInt xEnd = aX1 + aX2;
  TInt yEnd = aY1 + aY2;

  // Check if the entire rect is not on the display
  if (xEnd <= viewPortOffsetX || aX1 >= clipRectWidth || yEnd <= viewPortOffsetY || aY1 >= clipRectHeight) {
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
  if (xEnd > clipRectWidth) {
    xEnd = clipRectWidth;
  }

  // Don't end past the bottom edge
  if (yEnd > clipRectHeight) {
    yEnd = clipRectHeight;
  }

  // calculate actual width and height (even if unchanged)
  aX2 = xEnd - aX1;
  aY2 = yEnd - aY1;

  TUint8 *pixels;
  const TUint32 pitch = this->mPitch;
  const TUint32 yDest = aY1 * pitch;

  while (aX2--) {
    TInt h = aY2;
    pixels = &this->mPixels[yDest + aX1++];

    while (h > 3) {
      *pixels = aColor; pixels += pitch;
      *pixels = aColor; pixels += pitch;
      *pixels = aColor; pixels += pitch;
      *pixels = aColor; pixels += pitch;
      h -= 4;
    }

    while (h--) {
      *pixels = aColor; pixels += pitch;
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
    viewPortOffsetX = aViewPort->mOffsetX;
    viewPortOffsetY = aViewPort->mOffsetY;
    aX += viewPortOffsetX;
    aY += viewPortOffsetY;
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  }

  // Store viewport width/height
  const TInt clipRectWidth = clipRect.Width();
  const TInt clipRectHeight = clipRect.Height();

  TInt maxX = aX + r;
  TInt minX = aX - r;
  TInt maxY = aY + r;
  TInt minY = aY - r;

  // Circle is outside of the viewport
  if (maxX < viewPortOffsetX || minX > clipRectWidth || maxY < viewPortOffsetY || minY > clipRectHeight) {
    return;
  }

  TInt f = 1 - r;
  TInt ddF_x = 1;
  TInt ddF_y = -(r << 1);
  TInt x = 0;
  TInt y = r;
  TInt xx1, xx2, xx3, xx4, yy1, yy2, yy3, yy4;
  TUint32 yy1Dest, yy2Dest, yy3Dest, yy4Dest;
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

      xx1 = aX + x;
      xx2 = aX - x;
      xx3 = aX + y;
      xx4 = aX - y;
      yy1 = aY + y;
      yy2 = aY - y;
      yy3 = aY + x;
      yy4 = aY - x;
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
    if (aY >= viewPortOffsetX && aY < clipRectWidth) {
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

      xx1 = aX + x;
      xx2 = aX - x;
      xx3 = aX + y;
      xx4 = aX - y;
      yy1 = aY + y;
      yy2 = aY - y;
      yy3 = aY + x;
      yy4 = aY - x;
      yy1Dest = yy1 * pitch;
      yy2Dest = yy2 * pitch;
      yy3Dest = yy3 * pitch;
      yy4Dest = yy4 * pitch;
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
    viewPortOffsetX = aViewPort->mOffsetX;
    viewPortOffsetY = aViewPort->mOffsetY;
    aX += viewPortOffsetX;
    aY += viewPortOffsetY;
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  }

  // Store viewport width/height
  const TInt clipRectWidth = clipRect.Width();
  const TInt clipRectHeight = clipRect.Height();

  TInt maxX = aX + r;
  TInt minX = aX - r;
  TInt maxY = aY + r;
  TInt minY = aY - r;

  // Circle is outside of the viewport
  if (maxX < viewPortOffsetX || minX > clipRectWidth || maxY < viewPortOffsetY || minY > clipRectHeight) {
    return;
  }

  TInt f = 1 - r;
  TInt ddF_x = 1;
  TInt ddF_y = -(r << 1);
  TInt x = 0;
  TInt y = r;
  TUint w = (r << 1) + 1;
  TInt xx1, yy1, yy2;

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
      w = (x << 1) + 1;
      xx1 = aX - x;
      yy1 = aY + y;
      yy2 = aY - y;
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
      w = (y << 1) + 1;
      xx1 = aX - y;
      yy1 = aY + x;
      yy2 = aY - x;
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
    TInt16 xEnd = xx1 + w;

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
      w = xEnd - xx1;
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
      w = (x << 1) + 1;
      xx1 = aX - x;
      xEnd = xx1 + w;
      yy1 = aY + y;
      yy2 = aY - y;

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
        w = xEnd - xx1;

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
          w = xEnd - xx1;
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
      w = (y << 1) + 1;
      xx1 = aX - y;
      xEnd = xx1 + w;
      yy1 = aY + x;
      yy2 = aY - x;
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
        w = xEnd - xx1;

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
          w = xEnd - xx1;
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
