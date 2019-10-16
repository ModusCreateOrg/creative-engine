#include "BBitmap.h"
#include <Display/Display.h>
#include <cstring>
#include "Panic.h"
#include "BFont.h"
#include <strings.h>
#include <cmath>

#ifndef __XTENSA__

#include <cstdio>

#endif

#define RLE

TUint32 (BBitmap::*ReadPixelByDepth)(TInt, TInt) = ENull;

void BBitmap::CheckScreenDepth() {
  if (gDisplay.renderBitmap->mDepth != 32) {
    Panic( "Using 32 bit color in %i bit screen depth mode!\n", gDisplay.renderBitmap->mDepth);
  }
}

BBitmap::BBitmap(
  TUint aWidth, TUint aHeight, TUint aDepth, TUint16 aMemoryType) {
  mROM              = EFalse;
  mWidth            = aWidth;
  mHeight           = aHeight;
  mDepth            = aDepth;
  mPitch            = mWidth;
  mColors           = 256;
  mPalette          = new TRGB[mColors];
  mPixels = (TUint32 *) AllocMem((mWidth * mHeight) * sizeof(TUint32), aMemoryType);
  mTransparentColor = -1;

  mDimensions.x1 = mDimensions.y1 = 0;
  mDimensions.x2 = mWidth - 1;
  mDimensions.y2 = mHeight - 1;

  for (TInt i = 0; i < 256; i++) {
    mColorsUsed[i] = ENull;
  }

  ReadPixelByDepth = gDisplay.renderBitmap && gDisplay.renderBitmap->Depth() != mDepth
    ? &BBitmap::ReadPixelColor
    : &BBitmap::ReadPixel;
}

/**
 * construct a BBitmap from ROM definition
 */
struct ROMBitmap {
  TUint16 width, height, depth, bytesPerRow, paletteSize;
  TUint8  palette[1];
};

static TBool once = EFalse;

BBitmap::BBitmap(TAny *aROM, TUint16 aMemoryType) {
  auto *bmp = (ROMBitmap *) aROM;

  for (TInt i = 0; i < 256; i++) {
    mColorsUsed[i] = EFalse;
  }

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

  mPixels = (TUint32 *) AllocMem((mWidth * mHeight) * sizeof(TUint32), aMemoryType);
  if (!mPixels) {
    Panic("Cannot allocate mPixels\n");
  }
#ifdef RLE
  TUint32 *dst   = mPixels;
  TInt   length = mWidth * mHeight;
  while (length > 0) {
    TInt8 count = *ptr++;
    if (count & 0x80) {
      count &= 0x7f;
      count++;
#ifdef DEBUGME
      if (!once) {
        printf("rle run ");
      }
#endif
      for (TUint8 i = 0; i < count; i++) {
#ifdef DEBUGME
        if (!once) {
          printf("%02x ", *ptr);
        }
#endif
        TUint32 color = *ptr++;
        mColorsUsed[color] = ETrue;
        *dst++ = color;
        length--;
      }
#ifdef DEBUGME
      if (!once) {
        printf(" => rle %d length %d\n", count, length);
      }
#endif
    } else {
      count &= 0x7f;
      count++;
      TUint32      byte = *ptr++;
      mColorsUsed[byte] = ETrue;
      for (TUint8 i    = 0; i < count; i++) {
        *dst++ = byte;
        length--;
      }
#ifdef DEBUGME
      if (!once) {
        printf("rle dup %3d %02x length %d\n", count, byte, length);
      }
#endif
    }
  }
  once = ETrue;
#else
  TUint32 *dst = mPixels;

  for (TInt i = 0; i < mHeight * mPitch; i++) {
    TUint32 color = *ptr++;
    mColorsUsed[color] = ETrue;
    *dst++ = color;
  }
#endif

  ReadPixelByDepth = gDisplay.renderBitmap && gDisplay.renderBitmap->Depth() != mDepth
    ? &BBitmap::ReadPixelColor
    : &BBitmap::ReadPixel;
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
  printf("mWidth: %d, mHeight: %d, mDepth: %d, mPitch: %d\n", mWidth, mHeight,
      mDepth, mPitch);
  printf("mPixels: %s\n", mPixels);
  printf("mColors: %d, mPalette: %p\n", mColors, mPalette);
  for (TInt c = 0; c < mColors; c++) {
    printf("%3d ", c);
    mPalette[c].Dump();
  }
#endif
#endif
}

TInt BBitmap::NextUnusedColor() {
  for (TInt i = 0; i < 256; i++) {
    if (!mColorsUsed[i]) {
      return i;
    }
  }
  return -1;
}

TInt BBitmap::FindColor(const TRGB &aColor) {
  for (TInt ndx = 0; ndx < 256; ndx++) {
    if (mColorsUsed[ndx] && mPalette[ndx] == aColor) {
      return ndx;
    }
  }
  return -1;
}

// add aStartColor (it's an offset) to each pixel
// copy palette from 0 to aStartColor (index) for the number of colors
void BBitmap::Remap(BBitmap *aOther) {
  if (gDisplay.renderBitmap->Depth() != mDepth) {
    return;
  }

  TRGB transparent(255, 0, 255);
  mTransparentColor = -1;

  TInt16    remap[256];
  for (TInt i       = 0; i < 256; i++) {
    remap[i] = -1;
  }

  for (TInt y = 0; y < mHeight; y++) {
    for (TInt x = 0; x < mWidth; x++) {
      TInt pixel = ReadPixel(x, y);
      if (remap[pixel] != -1) {
        WritePixel(x, y, remap[pixel]);
      } else {
        TRGB &color = ReadColor(x, y);
        TInt found  = aOther->FindColor(color);
        if (found != -1) {
          remap[pixel] = found;
          if (mTransparentColor == -1 && color == transparent) {
            mTransparentColor = found;
          }
          WritePixel(x, y, found);
        } else {
          found = aOther->NextUnusedColor();
          if (found == -1) {
            Panic("Can't remap BBitmap: out of colors\n");
          }

          if (mTransparentColor == -1 && color == transparent) {
            mTransparentColor = found;
          }
          remap[pixel] = found;
          WritePixel(x, y, found);
          aOther->UseColor(found);
          aOther->SetColor(found, color);
        }
      }
    }
  }
}

TInt BBitmap::CountUsedColors() {
  TInt      count = 0;
  for (TInt i     = 0; i < 256; i++) {
    if (mColorsUsed[i]) {
      count++;
    }
  }
  return count;
}

TInt BBitmap::CountColors() {
  TBool     used[256];
  TInt      count = 0;
  for (TInt i     = 0; i < 256; i++) {
    used[i] = EFalse;
  }
  for (TInt y     = 0; y < mHeight; y++) {
    for (TInt x = 0; x < mWidth; x++) {
      TUint8 color = ReadPixel(x, y);
      if (!used[color]) {
        used[color] = ETrue;
        count++;
      }
    }
  }
  return count;
}

void BBitmap::SetPalette(TRGB aPalette[], TInt aIndex, TInt aCount) {
  TInt      cnt = MIN(mColors, aCount);
  for (TInt i   = 0; i < cnt; i++) {
    mPalette[aIndex + i].Set(aPalette[aIndex + i]);
  }
}

TBool BBitmap::DrawBitmap(BViewPort *aViewPort, BBitmap *aSrcBitmap,
                          TRect aSrcRect, TInt aX, TInt aY, TUint32 aFlags) {
  TUint32 *pixels;
  TRect clipRect, spriteRect;
  TInt nextRow,
       viewPortOffsetX = 0,
       viewPortOffsetY = 0,
       incX = 1,
       incY = 1;

  if (aViewPort) {
    aViewPort->GetRect(clipRect);
    viewPortOffsetX = TInt(round(aViewPort->mOffsetX));
    viewPortOffsetY = TInt(round(aViewPort->mOffsetY));
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
  }

  TInt destX = MAX(0, aX) + viewPortOffsetX,
       destY = MAX(0, aY) + viewPortOffsetY;

  // rotations (x, y are switched)
  if (aFlags & (DRAW_ROTATE_LEFT | DRAW_ROTATE_RIGHT)) {
    if (aFlags & DRAW_FLOPPED) {
      if (aFlags & DRAW_FLIPPED) {
        if (aFlags & DRAW_ROTATE_LEFT) {
          // rotate left, flipped, flopped = rotate right
          aFlags = DRAW_ROTATE_RIGHT;
        } else if (aFlags & DRAW_ROTATE_RIGHT) {
          // rotate right, flipped, flopped = rotate left
          aFlags = DRAW_ROTATE_LEFT;
        }
      } else {
        if (aFlags & DRAW_ROTATE_LEFT) {
          // rotate left, flopped = rotate right, flipped
          aFlags = DRAW_ROTATE_RIGHT | DRAW_FLIPPED;
        } else if (aFlags & DRAW_ROTATE_RIGHT) {
          // rotate right, flopped = rotate left, flipped
          aFlags = DRAW_ROTATE_LEFT | DRAW_FLIPPED;
        }
      }
    }

    if (aFlags & DRAW_FLIPPED) {
      if (aFlags & DRAW_ROTATE_LEFT) {
        // flip and rotate left
        incX = -1;
        incY = -1;
        destX = MIN(clipRect.x2, aX + aSrcRect.Width() - 1 + viewPortOffsetX);
        destY = MIN(clipRect.y2, aY + aSrcRect.Height() - 1 + viewPortOffsetY);
        spriteRect.Set(
          aSrcRect.y1 - MIN(0, clipRect.Width() - aX - aSrcRect.Width()),
          aSrcRect.x1 - MIN(0, clipRect.Height() - aY - aSrcRect.Height()),
          aSrcRect.y2 - MAX(0, clipRect.x1 - aX - viewPortOffsetX - 1),
          aSrcRect.x2 - MAX(0, clipRect.y1 - aY - viewPortOffsetY - 1)
        );
      } else {
        // flip and rotate right
        spriteRect.Set(
          aSrcRect.y1 + MAX(0, clipRect.x1 - aX - viewPortOffsetX),
          aSrcRect.x1 + MAX(0, clipRect.y1 - aY - viewPortOffsetY),
          aSrcRect.y2 + MIN(0, clipRect.Width() - aX - aSrcRect.Width() + 1),
          aSrcRect.x2 + MIN(0, clipRect.Height() - aY - aSrcRect.Height() + 1)
        );
      }
    } else if (aFlags & DRAW_ROTATE_LEFT) {
      // rotate left
      incY = -1;
      destY = MIN(clipRect.y2, aY + aSrcRect.Height() - 1 + viewPortOffsetY);
      spriteRect.Set(
        aSrcRect.y1 - MIN(0, clipRect.Width() - aX - aSrcRect.Width()),
        aSrcRect.x1 - MIN(0, clipRect.Height() - aY - aSrcRect.Height()),
        aSrcRect.y2 - MAX(0, clipRect.x1 - aX - viewPortOffsetX - 1),
        aSrcRect.x2 - MAX(0, clipRect.y1 - aY - viewPortOffsetY - 1)
      );
    } else {
      // rotate right
      incX = -1;
      destX = MIN(clipRect.x2, aX + aSrcRect.Width() - 1 + viewPortOffsetX);
      spriteRect.Set(
        aSrcRect.y1 - MIN(0, clipRect.Width() - aX - aSrcRect.Width()),
        aSrcRect.x1 + MAX(0, clipRect.y1 - aY - viewPortOffsetY),
        aSrcRect.y2 - MAX(0, clipRect.x1 - aX - viewPortOffsetX - 1),
        aSrcRect.x2 + MIN(0, clipRect.Height() - aY - aSrcRect.Height() + 1)
      );
    }

    if (spriteRect.Width() == 1 || spriteRect.Height() == 1) {
      return EFalse;
    }

    pixels = &this->mPixels[destY * mPitch + destX];
    nextRow = (mPitch * incY) - ((spriteRect.Width() - 1) * incX);

    for (TInt y = spriteRect.y1; y < spriteRect.y2; y++, pixels += nextRow) {
      for (TInt x = spriteRect.x1; x < spriteRect.x2; x++, pixels += incX) {
        *pixels = (aSrcBitmap->*ReadPixelByDepth)(y, x);
      }
    }
  } else {
    // no rotations
    if (aFlags & DRAW_FLIPPED) {
      if (aFlags & DRAW_FLOPPED) {
        // flipped and flopped
        incX = -1;
        incY = -1;
        destX = MIN(clipRect.x2, aX + aSrcRect.Width() - 1 + viewPortOffsetX);
        destY = MIN(clipRect.y2, aY + aSrcRect.Height() - 1 + viewPortOffsetY);
        spriteRect.Set(
          aSrcRect.x1 - MIN(0, clipRect.Width() - aX - aSrcRect.Width()),
          aSrcRect.y1 - MIN(0, clipRect.Height() - aY - aSrcRect.Height()),
          aSrcRect.x2 - MAX(0, clipRect.x1 - aX - viewPortOffsetX - 1),
          aSrcRect.y2 - MAX(0, clipRect.y1 - aY - viewPortOffsetY - 1)
        );
      } else {
        // flipped
        incX = -1;
        destX = MIN(clipRect.x2, aX + aSrcRect.Width() - 1 + viewPortOffsetX);
        spriteRect.Set(
          aSrcRect.x1 - MIN(0, clipRect.Width() - aX - aSrcRect.Width()),
          aSrcRect.y1 + MAX(0, clipRect.y1 - aY - viewPortOffsetY),
          aSrcRect.x2 - MAX(0, clipRect.x1 - aX - viewPortOffsetX - 1),
          aSrcRect.y2 + MIN(0, clipRect.Height() - aY - aSrcRect.Height() + 1)
        );
      }
    } else if (aFlags & DRAW_FLOPPED) {
      // flopped
      incY = -1;
      destY = MIN(clipRect.y2, aY + aSrcRect.Height() - 1 + viewPortOffsetY);
      spriteRect.Set(
        aSrcRect.x1 + MAX(0, clipRect.x1 - aX - viewPortOffsetX),
        aSrcRect.y1 - MIN(0, clipRect.Height() - aY - aSrcRect.Height()),
        aSrcRect.x2 + MIN(0, clipRect.Width() - aX - aSrcRect.Width() + 1),
        aSrcRect.y2 - MAX(0, clipRect.y1 - aY - viewPortOffsetY - 1)
      );
    } else {
      // normal
      spriteRect.Set(
        aSrcRect.x1 + MAX(0, clipRect.x1 - aX - viewPortOffsetX),
        aSrcRect.y1 + MAX(0, clipRect.y1 - aY - viewPortOffsetY),
        aSrcRect.x2 + MIN(0, clipRect.Width() - aX - aSrcRect.Width() + 1),
        aSrcRect.y2 + MIN(0, clipRect.Height() - aY - aSrcRect.Height() + 1)
      );
    }

    if (spriteRect.Width() == 1 || spriteRect.Height() == 1) {
      return EFalse;
    }

    pixels = &this->mPixels[destY * mPitch + destX];
    nextRow = (mPitch * incY) - ((spriteRect.Width() - 1) * incX);

    for (TInt y = spriteRect.y1; y < spriteRect.y2; y++, pixels += nextRow) {
      for (TInt x = spriteRect.x1; x < spriteRect.x2; x++, pixels += incX) {
        *pixels = (aSrcBitmap->*ReadPixelByDepth)(x, y);
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

  if (mDepth == aOther->mDepth) {
    memcpy(mPixels, aOther->mPixels, mWidth * mHeight * sizeof(TUint32));
    return;
  }

  if (mDepth == 32) {
    TUint32 *pixels = &mPixels[0];

    for (TInt y = 0; y < mHeight; y++) {
      for (TInt x = 0; x < mWidth; x++, pixels++) {
        *pixels = aOther->ReadColor(x, y).rgb888();
      }
    }
  }
}

TBool BBitmap::DrawBitmapTransparent(BViewPort *aViewPort, BBitmap *aSrcBitmap, TRect aSrcRect, TInt aX, TInt aY, TUint32 aFlags) {
  const TInt t = gDisplay.renderBitmap->Depth() != aSrcBitmap->mDepth
    ? aSrcBitmap->mPalette[aSrcBitmap->mTransparentColor].rgb888()
    : aSrcBitmap->mTransparentColor;

  TUint32 *pixels;
  TRect clipRect, spriteRect;
  TInt nextRow,
       viewPortOffsetX = 0,
       viewPortOffsetY = 0,
       incX = 1,
       incY = 1;

  if (aViewPort) {
    aViewPort->GetRect(clipRect);
    viewPortOffsetX = TInt(round(aViewPort->mOffsetX));
    viewPortOffsetY = TInt(round(aViewPort->mOffsetY));
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
  }

  TInt destX = MAX(0, aX) + viewPortOffsetX,
       destY = MAX(0, aY) + viewPortOffsetY;

  // rotations (x, y are switched)
  if (aFlags & (DRAW_ROTATE_LEFT | DRAW_ROTATE_RIGHT)) {
    if (aFlags & DRAW_FLOPPED) {
      if (aFlags & DRAW_FLIPPED) {
        if (aFlags & DRAW_ROTATE_LEFT) {
          // rotate left, flipped, flopped = rotate right
          aFlags = DRAW_ROTATE_RIGHT;
        } else if (aFlags & DRAW_ROTATE_RIGHT) {
          // rotate right, flipped, flopped = rotate left
          aFlags = DRAW_ROTATE_LEFT;
        }
      } else {
        if (aFlags & DRAW_ROTATE_LEFT) {
          // rotate left, flopped = rotate right, flipped
          aFlags = DRAW_ROTATE_RIGHT | DRAW_FLIPPED;
        } else if (aFlags & DRAW_ROTATE_RIGHT) {
          // rotate right, flopped = rotate left, flipped
          aFlags = DRAW_ROTATE_LEFT | DRAW_FLIPPED;
        }
      }
    }

    if (aFlags & DRAW_FLIPPED) {
      if (aFlags & DRAW_ROTATE_LEFT) {
        // flip and rotate left
        incX = -1;
        incY = -1;
        destX = MIN(clipRect.x2, aX + aSrcRect.Width() - 1 + viewPortOffsetX);
        destY = MIN(clipRect.y2, aY + aSrcRect.Height() - 1 + viewPortOffsetY);
        spriteRect.Set(
          aSrcRect.y1 - MIN(0, clipRect.Width() - aX - aSrcRect.Width()),
          aSrcRect.x1 - MIN(0, clipRect.Height() - aY - aSrcRect.Height()),
          aSrcRect.y2 - MAX(0, clipRect.x1 - aX - viewPortOffsetX - 1),
          aSrcRect.x2 - MAX(0, clipRect.y1 - aY - viewPortOffsetY - 1)
        );
      } else {
        // flip and rotate right
        spriteRect.Set(
          aSrcRect.y1 + MAX(0, clipRect.x1 - aX - viewPortOffsetX),
          aSrcRect.x1 + MAX(0, clipRect.y1 - aY - viewPortOffsetY),
          aSrcRect.y2 + MIN(0, clipRect.Width() - aX - aSrcRect.Width() + 1),
          aSrcRect.x2 + MIN(0, clipRect.Height() - aY - aSrcRect.Height() + 1)
        );
      }
    } else if (aFlags & DRAW_ROTATE_LEFT) {
      // rotate left
      incY = -1;
      destY = MIN(clipRect.y2, aY + aSrcRect.Height() - 1 + viewPortOffsetY);
      spriteRect.Set(
        aSrcRect.y1 - MIN(0, clipRect.Width() - aX - aSrcRect.Width()),
        aSrcRect.x1 - MIN(0, clipRect.Height() - aY - aSrcRect.Height()),
        aSrcRect.y2 - MAX(0, clipRect.x1 - aX - viewPortOffsetX - 1),
        aSrcRect.x2 - MAX(0, clipRect.y1 - aY - viewPortOffsetY - 1)
      );
    } else {
      // rotate right
      incX = -1;
      destX = MIN(clipRect.x2, aX + aSrcRect.Width() - 1 + viewPortOffsetX);
      spriteRect.Set(
        aSrcRect.y1 - MIN(0, clipRect.Width() - aX - aSrcRect.Width()),
        aSrcRect.x1 + MAX(0, clipRect.y1 - aY - viewPortOffsetY),
        aSrcRect.y2 - MAX(0, clipRect.x1 - aX - viewPortOffsetX - 1),
        aSrcRect.x2 + MIN(0, clipRect.Height() - aY - aSrcRect.Height() + 1)
      );
    }

    if (spriteRect.Width() == 1 || spriteRect.Height() == 1) {
      return EFalse;
    }

    pixels = &this->mPixels[destY * mPitch + destX];
    nextRow = (mPitch * incY) - ((spriteRect.Width() - 1) * incX);

    for (TInt y = spriteRect.y1; y < spriteRect.y2; y++, pixels += nextRow) {
      for (TInt x = spriteRect.x1; x < spriteRect.x2; x++, pixels += incX) {
        TUint32 pix = (aSrcBitmap->*ReadPixelByDepth)(y, x);
        if (pix != t) {
          *pixels = pix;
        }
      }
    }
  } else {
    // no rotations
    if (aFlags & DRAW_FLIPPED) {
      if (aFlags & DRAW_FLOPPED) {
        // flipped and flopped
        incX = -1;
        incY = -1;
        destX = MIN(clipRect.x2, aX + aSrcRect.Width() - 1 + viewPortOffsetX);
        destY = MIN(clipRect.y2, aY + aSrcRect.Height() - 1 + viewPortOffsetY);
        spriteRect.Set(
          aSrcRect.x1 - MIN(0, clipRect.Width() - aX - aSrcRect.Width()),
          aSrcRect.y1 - MIN(0, clipRect.Height() - aY - aSrcRect.Height()),
          aSrcRect.x2 - MAX(0, clipRect.x1 - aX - viewPortOffsetX - 1),
          aSrcRect.y2 - MAX(0, clipRect.y1 - aY - viewPortOffsetY - 1)
        );
      } else {
        // flipped
        incX = -1;
        destX = MIN(clipRect.x2, aX + aSrcRect.Width() - 1 + viewPortOffsetX);
        spriteRect.Set(
          aSrcRect.x1 - MIN(0, clipRect.Width() - aX - aSrcRect.Width()),
          aSrcRect.y1 + MAX(0, clipRect.y1 - aY - viewPortOffsetY),
          aSrcRect.x2 - MAX(0, clipRect.x1 - aX - viewPortOffsetX - 1),
          aSrcRect.y2 + MIN(0, clipRect.Height() - aY - aSrcRect.Height() + 1)
        );
      }
    } else if (aFlags & DRAW_FLOPPED) {
      // flopped
      incY = -1;
      destY = MIN(clipRect.y2, aY + aSrcRect.Height() - 1 + viewPortOffsetY);
      spriteRect.Set(
        aSrcRect.x1 + MAX(0, clipRect.x1 - aX - viewPortOffsetX),
        aSrcRect.y1 - MIN(0, clipRect.Height() - aY - aSrcRect.Height()),
        aSrcRect.x2 + MIN(0, clipRect.Width() - aX - aSrcRect.Width() + 1),
        aSrcRect.y2 - MAX(0, clipRect.y1 - aY - viewPortOffsetY - 1)
      );
    } else {
      // normal
      spriteRect.Set(
        aSrcRect.x1 + MAX(0, clipRect.x1 - aX - viewPortOffsetX),
        aSrcRect.y1 + MAX(0, clipRect.y1 - aY - viewPortOffsetY),
        aSrcRect.x2 + MIN(0, clipRect.Width() - aX - aSrcRect.Width() + 1),
        aSrcRect.y2 + MIN(0, clipRect.Height() - aY - aSrcRect.Height() + 1)
      );
    }

    if (spriteRect.Width() == 1 || spriteRect.Height() == 1) {
      return EFalse;
    }

    pixels = &this->mPixels[destY * mPitch + destX];
    nextRow = (mPitch * incY) - ((spriteRect.Width() - 1) * incX);

    for (TInt y = spriteRect.y1; y < spriteRect.y2; y++, pixels += nextRow) {
      for (TInt x = spriteRect.x1; x < spriteRect.x2; x++, pixels += incX) {
        TUint32 pix = (aSrcBitmap->*ReadPixelByDepth)(x, y);
        if (pix != t) {
          *pixels = pix;
        }
      }
    }
  }

  return ETrue;
}

TBool BBitmap::DrawStringShadow(BViewPort *aViewPort, const char *aStr,
                                const BFont *aFont, TInt aX, TInt aY, TUint8 aFgColor, TUint8 aShadowColor,
                                TInt8 aBgColor, TInt aLetterSpacing) {
  if (gDisplay.renderBitmap->mDepth == 32) {
    return DrawStringShadow(aViewPort, aStr, aFont, aX, aY, mPalette[aFgColor].rgb888(), mPalette[aShadowColor].rgb888(), TInt32(aBgColor), aLetterSpacing);
  }
  return DrawStringShadow(aViewPort, aStr, aFont, aX, aY, TUint32(aFgColor), TUint32(aShadowColor), TInt32(aBgColor), aLetterSpacing);
}

TBool BBitmap::DrawStringShadow(BViewPort *aViewPort, const char *aStr,
                                const BFont *aFont, TInt aX, TInt aY, TUint32 aFgColor, TUint32 aShadowColor,
                                TInt32 aBgColor, TInt aLetterSpacing) {
  return DrawString(aViewPort, aStr, aFont, aX, aY, aShadowColor, aBgColor, aLetterSpacing)
    ? DrawString(aViewPort, aStr, aFont, aX - 1, aY - 1, aFgColor, aBgColor, aLetterSpacing)
    : EFalse;
}

TBool BBitmap::DrawStringShadow(BViewPort *aViewPort, const char *aStr, const BFont *aFont,
                                TInt aX, TInt aY, const TRGB &aFgColor, const TRGB &aShadowColor,
                                const TRGB &aBgColor, TInt aLetterSpacing) {
  const TInt32 bg = aBgColor == -1 ? -1 : aBgColor.rgb888();
  return DrawString(aViewPort, aStr, aFont, aX, aY, aShadowColor.rgb888(), bg, aLetterSpacing)
    ? DrawString(aViewPort, aStr, aFont, aX - 1, aY - 1, aFgColor.rgb888(), bg, aLetterSpacing)
    : EFalse;
}

TBool BBitmap::DrawString(BViewPort *aViewPort, const char *aStr,
                          const BFont *aFont, TInt aX, TInt aY, TUint8 aFgColor, TInt8 aBgColor,
                          TInt aLetterSpacing) {
  if (gDisplay.renderBitmap->mDepth == 32) {
    return DrawString(aViewPort, aStr, aFont, aX, aY, mPalette[aFgColor].rgb888(), TInt32(aBgColor), aLetterSpacing);
  }
  return DrawString(aViewPort, aStr, aFont, aX, aY, TUint32(aFgColor), TInt32(aBgColor), aLetterSpacing);
}

TBool BBitmap::DrawString(BViewPort *aViewPort, const char *aStr, const BFont *aFont,
                          TInt aX, TInt aY, const TRGB &aFgColor, const TRGB &aBgColor,
                          TInt aLetterSpacing) {
  const TInt32 bg = aBgColor == -1 ? -1 : aBgColor.rgb888();
  return DrawString(aViewPort, aStr, aFont, aX, aY, aFgColor.rgb888(), bg, aLetterSpacing);
}

TBool BBitmap::DrawString(BViewPort *aViewPort, const char *aStr,
                          const BFont *aFont, TInt aX, TInt aY, TUint32 aFgColor, TInt32 aBgColor,
                          TInt aLetterSpacing) {
  const TInt    fontWidth   = aFont->mWidth,
                fontHeight  = aFont->mHeight,
                charOffset  = fontWidth + aLetterSpacing;
  const TBool   drawBg      = aBgColor != -1;
  TBool         drawn       = EFalse;
  BBitmap       *fontBitmap = aFont->mBitmap;

  TUint32 *pixels;
  TRect clipRect, charRect;
  TInt nextRow,
       viewPortOffsetX = 0,
       viewPortOffsetY = 0;

  if (aViewPort) {
    aViewPort->GetRect(clipRect);
    viewPortOffsetX = TInt(round(aViewPort->mOffsetX));
    viewPortOffsetY = TInt(round(aViewPort->mOffsetY));
  } else {
    clipRect.Set(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
  }

  while (*aStr) {
    TInt destX = MAX(0, aX) + viewPortOffsetX,
         destY = MAX(0, aY) + viewPortOffsetY;

    // Get character
    const char c   = *aStr++;
    const TInt col = (c % 16) * fontWidth;
    const TInt row = (c / 16) * fontHeight;

    charRect.Set(
      col + MAX(0, clipRect.x1 - aX - viewPortOffsetX),
      row + MAX(0, clipRect.y1 - aY - viewPortOffsetY),
      col + fontWidth + MIN(0, clipRect.Width() - aX - fontWidth),
      row + fontHeight + MIN(0, clipRect.Height() - aY - fontHeight)
    );

    if (charRect.Height() <= 1) {
      return EFalse;
    }

    if (charRect.Width() <= 1) {
      drawn |= EFalse;
      aX += charOffset;
      continue;
    }

    pixels = &mPixels[destY * mPitch + destX];
    nextRow = mPitch - (charRect.Width() - 1);

    for (TInt y = charRect.y1; y < charRect.y2; y++, pixels += nextRow) {
      for (TInt x = charRect.x1; x < charRect.x2; x++, pixels++) {
        TUint32 pix = (fontBitmap->*ReadPixelByDepth)(x, y);
        // Write background and foreground pixels
        if (pix == 0) {
          if (drawBg) {
            *pixels = TUint32(aBgColor);
          }
        } else {
          *pixels = aFgColor;
        }
      }
    }

    aX += charOffset;
    drawn |= ETrue;
  }

  return drawn;
}

void BBitmap::Clear(const TRGB &aColor) {
  BBitmap::CheckScreenDepth();

  const TInt len = mPitch * mHeight;

  if (aColor == 0) {
    memset(mPixels, 0, len * sizeof(TUint32));
    return;
  }

  for (TInt i = 0; i < len; i++) {
    WritePixel(i, 0, aColor);
  }
}

void BBitmap::Clear(TUint8 aIndex) {
  const TInt len = mPitch * mHeight;

  if (gDisplay.renderBitmap->mDepth == 32) {
    Clear(mPalette[aIndex]);
    return;
  }

  if (aIndex == 0) {
    memset(mPixels, aIndex, len * sizeof(TUint32));
    return;
  }

  for (TInt i = 0; i < len; i++) {
    WritePixel(i, 0, aIndex);
  }
}

void BBitmap::WritePixel(TInt aX, TInt aY, const TRGB &aColor) {
  BBitmap::CheckScreenDepth();
  mPixels[aY * mPitch + aX] = aColor.rgb888();
}

void BBitmap::WritePixel(TInt aX, TInt aY, TUint8 aIndex) {
  if (gDisplay.renderBitmap->mDepth == 32) {
    WritePixel(aX, aY, mPalette[aIndex]);
    return;
  }
  mPixels[aY * mPitch + aX] = aIndex;
}

void BBitmap::SafeWritePixel(TInt aX, TInt aY, const TRGB &aColor) {
  BBitmap::CheckScreenDepth();

  if (mDimensions.PointInRect(aX, aY)) {
    WritePixel(aX, aY, aColor.rgb888());
  }
}

void BBitmap::SafeWritePixel(TInt aX, TInt aY, TUint8 aIndex) {
  if (mDimensions.PointInRect(aX, aY)) {
    if (gDisplay.renderBitmap->mDepth == 32) {
      WritePixel(aX, aY, mPalette[aIndex]);
      return;
    }
    WritePixel(aX, aY, aIndex);
  }
}

void BBitmap::DrawFastHLine(BViewPort *aViewPort, TInt aX, TInt aY, TUint aW, TUint8 aColor) {
  if (gDisplay.renderBitmap->mDepth == 32) {
    DrawFastHLine(aViewPort, aX, aY, aW, mPalette[aColor].rgb888());
    return;
  }
  DrawFastHLine(aViewPort, aX, aY, aW, TUint32(aColor));
}

void BBitmap::DrawFastHLine(BViewPort *aViewPort, TInt aX, TInt aY, TUint aW, TUint32 aColor) {
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
  const TInt clipRectWidth  = clipRect.Width() - 1 + viewPortOffsetX;
  const TInt clipRectHeight = clipRect.Height() - 1 + viewPortOffsetY;

  // Do y bounds checks
  if (aY < viewPortOffsetY || aY > clipRectHeight) {
    return;
  }

  // last x point + 1
  TInt xEnd = aX + aW;

  // Check if the entire line is not on the display
  if (xEnd < viewPortOffsetX || aX > clipRectWidth) {
    return;
  }

  // Don't start before the left edge
  if (aX < viewPortOffsetX) {
    aX = viewPortOffsetX;
  }

  // Don't end past the right edge
  if (xEnd > clipRectWidth) {
    xEnd = clipRectWidth + 1;
  }

  // calculate actual width (even if unchanged)
  aW = TUint(xEnd - aX);

  TUint32 *pixels = &mPixels[aY * mPitch + aX];

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
  if (gDisplay.renderBitmap->mDepth == 32) {
    DrawFastVLine(aViewPort, aX, aY, aH, mPalette[aColor].rgb888());
    return;
  }
  DrawFastVLine(aViewPort, aX, aY, aH, TUint32(aColor));
}

void BBitmap::DrawFastVLine(BViewPort *aViewPort, TInt aX, TInt aY, TUint aH, TUint32 aColor) {
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
  const TInt clipRectWidth  = clipRect.Width() - 1 + viewPortOffsetX;
  const TInt clipRectHeight = clipRect.Height() - 1 + viewPortOffsetY;

  // Do x bounds checks
  if (aX < viewPortOffsetX || aX > clipRectWidth) {
    return;
  }

  // last y point + 1
  TInt yEnd = aY + aH;

  // Check if the entire line is not on the display
  if (yEnd < viewPortOffsetY || aY > clipRectHeight) {
    return;
  }

  // Don't start before the top edge
  if (aY < viewPortOffsetY) {
    aY = viewPortOffsetY;
  }

  // Don't end past the bottom edge
  if (yEnd > clipRectHeight) {
    yEnd = clipRectHeight + 1;
  }

  // calculate actual height (even if unchanged)
  aH = TUint(yEnd - aY);

  TUint32 *pixels = &mPixels[aY * mPitch + aX];

  while (aH > 3) {
    *pixels = aColor;
    pixels += mPitch;
    *pixels = aColor;
    pixels += mPitch;
    *pixels = aColor;
    pixels += mPitch;
    *pixels = aColor;
    pixels += mPitch;
    aH -= 4;
  }

  while (aH > 0) {
    *pixels = aColor;
    pixels += mPitch;
    aH--;
  }
}

void BBitmap::DrawLine(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint8 aColor) {
  if (gDisplay.renderBitmap->mDepth == 32) {
    DrawLine(aViewPort, aX1, aY1, aX2, aY2, mPalette[aColor].rgb888());
    return;
  }
  DrawLine(aViewPort, aX1, aY1, aX2, aY2, TUint32(aColor));
}

void BBitmap::DrawLine(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint32 aColor) {
  // Draw simple lines if possible
  if (aY1 == aY2) {
    return DrawFastHLine(aViewPort, aX1, aY1, aX2 - aX1 + 1, aColor);
  } else if (aX1 == aX2) {
    return DrawFastVLine(aViewPort, aX1, aY1, aY2 - aY1 + 1, aColor);
  }

  // Bresenham's algorithm
  // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

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
  const TInt clipRectWidth  = clipRect.Width() - 1 + viewPortOffsetX;
  const TInt clipRectHeight = clipRect.Height() - 1 + viewPortOffsetY;
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

  const TFloat m = TFloat(aY2 - aY1) / (aX2 - aX1);

  if (steep) {
    if (aX1 < viewPortOffsetY || aX1 > clipRectHeight) {
      aX1 = aX1 < viewPortOffsetY ? viewPortOffsetY : clipRectHeight;
      const TInt tempDeltaX = MAX(1, aX2 - aX1);
      aY1 = TInt(-tempDeltaX * (m - ((TFloat) aY2 / tempDeltaX)));
    }

    if (aY1 < viewPortOffsetX || aY1 > clipRectWidth) {
      aY1 = aY1 < viewPortOffsetX ? viewPortOffsetX : clipRectWidth;
      const TInt tempDeltaY = aY2 - aY1;
      aX1 = MAX(viewPortOffsetY, TInt((-tempDeltaY + (m * aX2)) / m));
    }

    if (aX2 > clipRectHeight || aX2 < viewPortOffsetY ||
        aY2 > clipRectWidth || aY2 < viewPortOffsetX) {
      aY2 = aY2 < viewPortOffsetX ? viewPortOffsetX : MIN(aY2, clipRectWidth);
      aX2 = MIN(clipRectHeight, TInt(((aY2 - aY1) + (m * aX1)) / m));
    }

    // aY1 is X coord and aX1 is Y coord in this case
    for (; aX1 <= aX2; aX1++) {
      mPixels[aX1 * mPitch + aY1] = aColor;

      err -= dy;

      if (err < 0) {
        aY1 += ystep;
        err += dx;
      }
    }
  } else {
    if (aY1 < viewPortOffsetY || aY1 > clipRectHeight) {
      aY1 = aY1 < viewPortOffsetY ? viewPortOffsetY : clipRectHeight;
      const TInt tempDeltaY = aY2 - aY1;
      aX1 = TInt((-tempDeltaY + (m * aX2)) / m);
    }

    if (aX1 < viewPortOffsetX || aX1 > clipRectWidth) {
      aX1 = aX1 < viewPortOffsetX ? viewPortOffsetX : clipRectWidth;
      const TInt tempDeltaX = MAX(1, aX2 - aX1);
      aY1 = MAX(viewPortOffsetY, TInt(-tempDeltaX * (m - ((TFloat) aY2 / tempDeltaX))));
    }

    if (aY2 > clipRectHeight || aY2 < viewPortOffsetY ||
        aX2 > clipRectWidth || aX2 < viewPortOffsetX) {
      aY2 = aY2 < viewPortOffsetY ? viewPortOffsetY : MIN(aY2, clipRectHeight);
      aX2 = MIN(clipRectWidth , TInt(((aY2 - aY1) + (m * aX1)) / m));
    }


    // aX1 is X coord and aY1 is Y coord in this case
    for (; aX1 <= aX2; aX1++) {
      mPixels[aY1 * mPitch + aX1] = aColor;

      err -= dy;

      if (err < 0) {
        aY1 += ystep;
        err += dx;
      }
    }
  }
}

void BBitmap::DrawRect(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint8 aColor) {
  if (gDisplay.renderBitmap->mDepth == 32) {
    DrawRect(aViewPort, aX1, aY1, aX2, aY2, mPalette[aColor].rgb888());
    return;
  }
  DrawRect(aViewPort, aX1, aY1, aX2, aY2, TUint32(aColor));
}

void BBitmap::DrawRect(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint32 aColor) {
  const TInt width = aX2 - aX1 + 1;
  const TInt height = aY2 - aY1 + 1;
  DrawFastHLine(aViewPort, aX1, aY1, width, aColor);
  DrawFastHLine(aViewPort, aX1, aY2, width, aColor);
  DrawFastVLine(aViewPort, aX1, aY1, height, aColor);
  DrawFastVLine(aViewPort, aX2, aY1, height, aColor);
}

void BBitmap::FillRect(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint8 aColor) {
  if (gDisplay.renderBitmap->mDepth == 32) {
    FillRect(aViewPort, aX1, aY1, aX2, aY2, mPalette[aColor].rgb888());
    return;
  }
  FillRect(aViewPort, aX1, aY1, aX2, aY2, TUint32(aColor));
}

void BBitmap::FillRect(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2, TInt aY2, TUint32 aColor) {
  const TInt width = ABS(aX2 - aX1 + 1);
  const TInt height = ABS(aY2 - aY1 + 1);

  if (width > height) {
    TInt h = height;
    while(h--) {
      DrawFastHLine(aViewPort, aX1, aY1++, width, aColor);
    }
    return;
  }

  TInt w = width;
  while(w--) {
    DrawFastVLine(aViewPort, aX1++, aY1, height, aColor);
  }
}

void BBitmap::DrawCircle(BViewPort *aViewPort, TInt aX, TInt aY, TUint r, TUint8 aColor) {
  if (gDisplay.renderBitmap->mDepth == 32) {
    DrawCircle(aViewPort, aX, aY, r, mPalette[aColor].rgb888());
    return;
  }
  DrawCircle(aViewPort, aX, aY, r, TUint32(aColor));
}

void BBitmap::DrawCircle(BViewPort *aViewPort, TInt aX, TInt aY, TUint r, TUint32 aColor) {
  if (gDisplay.renderBitmap->mDepth == 32) {
    aColor = mPalette[aColor].rgb888();
  }

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
  const TInt clipRectWidth  = clipRect.Width() + viewPortOffsetX;
  const TInt clipRectHeight = clipRect.Height() + viewPortOffsetY;

  TInt maxX = aX + r;
  TInt minX = aX - r;
  TInt maxY = aY + r;
  TInt minY = aY - r;

  // Circle is outside of the viewport
  if (maxX < viewPortOffsetX || minX >= clipRectWidth ||
      maxY < viewPortOffsetY || minY >= clipRectHeight) {
    return;
  }

  TInt          f     = 1 - r;
  TInt          ddF_x = 1;
  TInt          ddF_y = -(r << 1);
  TInt          x     = 0;
  TInt          y     = r;
  TInt          xx1, xx2, xx3, xx4, yy1, yy2, yy3, yy4;
  TUint32       yy1Dest, yy2Dest, yy3Dest, yy4Dest;

  // Circle is inside the viewport
  if (minX >= viewPortOffsetX && maxX < clipRectWidth &&
      minY >= viewPortOffsetY && maxY < clipRectHeight) {
    // top and bottom center pixels
    mPixels[maxY * mPitch + aX] = aColor;
    mPixels[minY * mPitch + aX] = aColor;

    // left and right center pixels
    mPixels[aY * mPitch + maxX] = aColor;
    mPixels[aY * mPitch + minX] = aColor;

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
      yy1Dest = yy1 * mPitch;
      yy2Dest = yy2 * mPitch;
      yy3Dest = yy3 * mPitch;
      yy4Dest = yy4 * mPitch;

      // top and bottom right arcs
      mPixels[yy1Dest + xx1] = aColor;
      mPixels[yy2Dest + xx1] = aColor;
      mPixels[yy3Dest + xx3] = aColor;
      mPixels[yy4Dest + xx3] = aColor;

      // top and bottom left arcs
      mPixels[yy1Dest + xx2] = aColor;
      mPixels[yy2Dest + xx2] = aColor;
      mPixels[yy3Dest + xx4] = aColor;
      mPixels[yy4Dest + xx4] = aColor;
    }
  } else {
    // Circle is partially inside the viewport
    TBool yy1Visible, yy2Visible, yy3Visible, yy4Visible;

    // top and bottom center pixels
    if (aX >= viewPortOffsetX && aX < clipRectWidth) {
      if (maxY < clipRectHeight) {
        mPixels[maxY * mPitch + aX] = aColor;
      }
      if (minY >= viewPortOffsetY) {
        mPixels[minY * mPitch + aX] = aColor;
      }
    }
    // left and right center pixels
    if (aY >= viewPortOffsetY && aY < clipRectHeight) {
      if (maxX < clipRectWidth) {
        mPixels[aY * mPitch + maxX] = aColor;
      }
      if (minX >= viewPortOffsetX) {
        mPixels[aY * mPitch + minX] = aColor;
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
      yy1Dest    = yy1 * mPitch;
      yy2Dest    = yy2 * mPitch;
      yy3Dest    = yy3 * mPitch;
      yy4Dest    = yy4 * mPitch;
      yy1Visible = yy1 >= viewPortOffsetY && yy1 < clipRectHeight;
      yy2Visible = yy2 >= viewPortOffsetY && yy2 < clipRectHeight;
      yy3Visible = yy3 >= viewPortOffsetY && yy3 < clipRectHeight;
      yy4Visible = yy4 >= viewPortOffsetY && yy4 < clipRectHeight;

      // top and bottom right arcs
      if (xx1 >= viewPortOffsetX && xx1 < clipRectWidth) {
        if (yy1Visible) {
          mPixels[yy1Dest + xx1] = aColor;
        }
        if (yy2Visible) {
          mPixels[yy2Dest + xx1] = aColor;
        }
      }
      if (xx3 >= viewPortOffsetX && xx3 < clipRectWidth) {
        if (yy3Visible) {
          mPixels[yy3Dest + xx3] = aColor;
        }
        if (yy4Visible) {
          mPixels[yy4Dest + xx3] = aColor;
        }
      }

      // top and bottom left arcs
      if (xx2 >= viewPortOffsetX && xx2 < clipRectWidth) {
        if (yy1Visible) {
          mPixels[yy1Dest + xx2] = aColor;
        }
        if (yy2Visible) {
          mPixels[yy2Dest + xx2] = aColor;
        }
      }
      if (xx4 >= viewPortOffsetX && xx4 < clipRectWidth) {
        if (yy3Visible) {
          mPixels[yy3Dest + xx4] = aColor;
        }
        if (yy4Visible) {
          mPixels[yy4Dest + xx4] = aColor;
        }
      }
    }
  }
}

void BBitmap::FillCircle(BViewPort *aViewPort, TInt aX, TInt aY, TUint r, TUint8 aColor) {
  if (gDisplay.renderBitmap->mDepth == 32) {
    FillCircle(aViewPort, aX, aY, r, mPalette[aColor].rgb888());
    return;
  }
  FillCircle(aViewPort, aX, aY, r, TUint32(aColor));
}

void BBitmap::FillCircle(BViewPort *aViewPort, TInt aX, TInt aY, TUint r, TUint32 aColor) {
  if (gDisplay.renderBitmap->mDepth == 32) {
    aColor = mPalette[aColor].rgb888();
  }

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
  const TInt clipRectWidth  = clipRect.Width() + viewPortOffsetX;
  const TInt clipRectHeight = clipRect.Height() + viewPortOffsetY;

  TInt maxX = aX + r;
  TInt minX = aX - r;
  TInt maxY = aY + r;
  TInt minY = aY - r;

  // Circle is outside of the viewport
  if (maxX < viewPortOffsetX || minX >= clipRectWidth ||
      maxY < viewPortOffsetY || minY >= clipRectHeight) {
    return;
  }

  TInt  f     = 1 - r;
  TInt  ddF_x = 1;
  TInt  ddF_y = -(r << 1);
  TUint x     = 0;
  TUint y     = r;
  TUint w     = (r << 1) + 1;
  TInt  xx1, yy1, yy2;


  // Circle is inside the viewport
  if (minX >= viewPortOffsetX && maxX < clipRectWidth &&
      minY >= viewPortOffsetY && maxY < clipRectHeight) {
    TUint32 *pixels1 = &mPixels[aY * mPitch + aX - r];
    TUint32 *pixels2;

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
      pixels1 = &mPixels[yy1 * mPitch + xx1];
      pixels2 = &mPixels[yy2 * mPitch + xx1];
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
      pixels1 = &mPixels[yy1 * mPitch + xx1];
      pixels2 = &mPixels[yy2 * mPitch + xx1];
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
    TUint32 *pixels;

    // Circle is partially inside the viewport
    xx1 = aX - r;

    // last x point + 1
    TInt xEnd = xx1 + w;

    // Central line
    if (aY >= viewPortOffsetY && aY < clipRectHeight &&
        xEnd >= viewPortOffsetX && xx1 < clipRectWidth) {
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
      pixels = &mPixels[aY * mPitch + xx1];

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
          pixels = &mPixels[yy1 * mPitch + xx1];
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
          pixels = &mPixels[yy2 * mPitch + xx1];
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
          pixels = &mPixels[yy1 * mPitch + xx1];
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
          pixels = &mPixels[yy2 * mPitch + xx1];
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
