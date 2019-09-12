#include "BBitmap.h"
#include <cstring>
#include "Panic.h"
#include "BFont.h"
#include <strings.h>
#include <cmath>

#ifndef __XTENSA__

#include <cstdio>

#endif

#define RLE

BBitmap::BBitmap(
  TUint aWidth, TUint aHeight, TUint aDepth, TUint16 aMemoryType) {
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

  for (TInt i = 0; i < 256; i++) {
    mColorsUsed[i] = ENull;
  }
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

  mPixels = (TUint8 *) AllocMem(mWidth * mHeight, aMemoryType);
  if (!mPixels) {
    Panic("Cannot allocate mPixels\n");
  }
#ifdef RLE
  TUint8 *dst   = mPixels;
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
        TUint8 color = *ptr++;
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
      TUint8      byte = *ptr++;
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
  TUint8 *dst = mPixels;

  for (TInt i = 0; i < mHeight * mPitch; i++) {
    TUint8 color = *ptr++;
    mColorsUsed[color] = ETrue;
    *dst++ = color;
  }
#endif
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

TInt BBitmap::FindColor(TRGB &aColor) {
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
  const TUint32 pitch = this->mPitch;
  const TInt t = aSrcBitmap->mTransparentColor;

  TUint8 *pixels;
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

    pixels = &this->mPixels[destY * pitch + destX];
    nextRow = (pitch * incY) - ((spriteRect.Width() - 1) * incX);

    for (TInt y = spriteRect.y1; y < spriteRect.y2; y++, pixels += nextRow) {
      for (TInt x = spriteRect.x1; x < spriteRect.x2; x++, pixels += incX) {
        *pixels = aSrcBitmap->ReadPixel(y, x);
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

    pixels = &this->mPixels[destY * pitch + destX];
    nextRow = (pitch * incY) - ((spriteRect.Width() - 1) * incX);

    for (TInt y = spriteRect.y1; y < spriteRect.y2; y++, pixels += nextRow) {
      for (TInt x = spriteRect.x1; x < spriteRect.x2; x++, pixels += incX) {
        *pixels = aSrcBitmap->ReadPixel(x, y);
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

TBool BBitmap::DrawBitmapTransparent(BViewPort *aViewPort, BBitmap *aSrcBitmap, TRect aSrcRect, TInt aX, TInt aY, TUint32 aFlags) {
  const TUint32 pitch = this->mPitch;
  const TInt t = aSrcBitmap->mTransparentColor;

  TUint8 *pixels;
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

    pixels = &this->mPixels[destY * pitch + destX];
    nextRow = (pitch * incY) - ((spriteRect.Width() - 1) * incX);

    for (TInt y = spriteRect.y1; y < spriteRect.y2; y++, pixels += nextRow) {
      for (TInt x = spriteRect.x1; x < spriteRect.x2; x++, pixels += incX) {
        TUint8 pix = aSrcBitmap->ReadPixel(y, x);
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

    pixels = &this->mPixels[destY * pitch + destX];
    nextRow = (pitch * incY) - ((spriteRect.Width() - 1) * incX);

    for (TInt y = spriteRect.y1; y < spriteRect.y2; y++, pixels += nextRow) {
      for (TInt x = spriteRect.x1; x < spriteRect.x2; x++, pixels += incX) {
        TUint8 pix = aSrcBitmap->ReadPixel(x, y);
        if (pix != t) {
          *pixels = pix;
        }
      }
    }
  }

  return ETrue;
}

TBool BBitmap::DrawStringShadow(BViewPort *aViewPort, const char *aStr,
                                const BFont *aFont, TInt aX, TInt aY, TInt aFgColor, TInt aShadowColor,
                                TInt aBgColor, TInt aLetterSpacing) {
  return DrawString(aViewPort, aStr, aFont, aX, aY, aShadowColor, aBgColor, aLetterSpacing)
    ? DrawString(aViewPort, aStr, aFont, aX - 1, aY - 1, aFgColor, aBgColor, aLetterSpacing)
    : EFalse;
}

TBool BBitmap::DrawString(BViewPort *aViewPort, const char *aStr,
                          const BFont *aFont, TInt aX, TInt aY, TInt aFgColor, TInt aBgColor,
                          TInt aLetterSpacing) {
  const TUint32 pitch       = this->mPitch;
  const TInt    fontWidth   = aFont->mWidth,
                fontHeight  = aFont->mHeight,
                charOffset  = fontWidth + aLetterSpacing;
  const TBool   drawBg      = aBgColor != -1;
  TBool         drawn       = EFalse;
  BBitmap       *fontBitmap = aFont->mBitmap;

  TUint8 *pixels;
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

    pixels = &this->mPixels[destY * pitch + destX];
    nextRow = pitch - (charRect.Width() - 1);

    for (TInt y = charRect.y1; y < charRect.y2; y++, pixels += nextRow) {
      for (TInt x = charRect.x1; x < charRect.x2; x++, pixels++) {
        TUint8 pix = fontBitmap->ReadPixel(x, y);
        // Write background and foreground pixels
        if (pix == 0) {
          if (drawBg) {
            *pixels = TUint8(aBgColor);
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

void BBitmap::Clear(TUint8 aColor) {
  memset(mPixels, aColor, mPitch * mHeight);
}

void BBitmap::DrawFastHLine(
  BViewPort *aViewPort, TInt aX, TInt aY, TUint aW, TUint8 aColor) {
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

void BBitmap::DrawFastVLine(
  BViewPort *aViewPort, TInt aX, TInt aY, TUint aH, TUint8 aColor) {
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

void BBitmap::DrawLine(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2,
                       TInt aY2, TUint8 aColor) {
  TInt viewPortOffsetX = 0;
  TInt viewPortOffsetY = 0;

  TUint8  *pixels;
  const TUint32 pitch = this->mPitch;

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

  // Draw simple lines if possible
  if (aY1 == aY2) {
    // Do y bounds checks
    if (aY1 < viewPortOffsetY || aY1 > clipRectHeight) {
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
      if (xEnd <= viewPortOffsetX || aX1 > clipRectWidth) {
        return;
      }

      // Don't start before the left edge
      if (aX1 < viewPortOffsetX) {
        aX1 = viewPortOffsetX;
      }

      // Don't end past the right edge
      if (xEnd > clipRectWidth) {
        xEnd = clipRectWidth + 1;
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
      if (xEnd <= viewPortOffsetX || aX1 > clipRectWidth) {
        return;
      }

      // Don't start before the left edge
      if (aX2 < viewPortOffsetX) {
        aX2 = viewPortOffsetX;
      }

      // Don't end past the right edge
      if (xEnd > clipRectWidth) {
        xEnd = clipRectWidth + 1;
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
      if (yEnd <= viewPortOffsetY || aY1 > clipRectHeight) {
        return;
      }

      // Don't start before the top edge
      if (aY1 < viewPortOffsetY) {
        aY1 = viewPortOffsetY;
      }

      // Don't end past the bottom edge
      if (yEnd > clipRectHeight) {
        yEnd = clipRectHeight + 1;
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
      if (yEnd <= viewPortOffsetY || aY2 > clipRectHeight) {
        return;
      }

      // Don't start before the top edge
      if (aY2 < viewPortOffsetY) {
        aY2 = viewPortOffsetY;
      }

      // Don't end past the bottom edge
      if (yEnd > clipRectHeight) {
        yEnd = clipRectHeight + 1;
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
      this->mPixels[aX1 * pitch + aY1] = aColor;

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
      this->mPixels[aY1 * pitch + aX1] = aColor;

      err -= dy;

      if (err < 0) {
        aY1 += ystep;
        err += dx;
      }
    }
  }
}

void BBitmap::DrawRect(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2,
                       TInt aY2, TUint8 aColor) {
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
  const TInt clipRectWidth  = clipRect.Width() - 1 + viewPortOffsetX;
  const TInt clipRectHeight = clipRect.Height() - 1 + viewPortOffsetY;

  // calculate boundaries
  TInt xMax  = MAX(viewPortOffsetX, aX1);
  TInt yDest = MAX(0, aY1) * pitch;
  aX2 = MIN(clipRectWidth + 1, aX2);
  w   = aX2 - xMax;

  // Draw horizontal lines
  if (aX2 > viewPortOffsetX && aX1 <= clipRectWidth) {
    // Draw rectangle's top side
    if (aY1 > viewPortOffsetY && aY1 <= clipRectHeight) {
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
    if (aY2 > viewPortOffsetY && aY2 <= clipRectHeight) {
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
  if (aY2 > viewPortOffsetY && aY1 <= clipRectHeight) {
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
    if (aX1 > viewPortOffsetX && aX1 <= clipRectWidth) {
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
    if (aX2 > viewPortOffsetX && aX2 <= clipRectWidth) {
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

void BBitmap::FillRect(BViewPort *aViewPort, TInt aX1, TInt aY1, TInt aX2,
                       TInt aY2, TUint8 aColor) {
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

  // Check if the entire rect is not on the display
  if (aX2 <= viewPortOffsetX || aX1 > clipRectWidth ||
      aY2 <= viewPortOffsetY || aY1 > clipRectHeight) {
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

void BBitmap::DrawCircle(
  BViewPort *aViewPort, TInt aX, TInt aY, TUint r, TUint8 aColor) {
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
  const TUint32 pitch = this->mPitch;

  // Circle is inside the viewport
  if (minX >= viewPortOffsetX && maxX < clipRectWidth &&
      minY >= viewPortOffsetY && maxY < clipRectHeight) {
    // top and bottom center pixels
    mPixels[maxY * pitch + aX] = aColor;
    mPixels[minY * pitch + aX] = aColor;

    // left and right center pixels
    mPixels[aY * pitch + maxX] = aColor;
    mPixels[aY * pitch + minX] = aColor;

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
        mPixels[maxY * pitch + aX] = aColor;
      }
      if (minY >= viewPortOffsetY) {
        mPixels[minY * pitch + aX] = aColor;
      }
    }
    // left and right center pixels
    if (aY >= viewPortOffsetY && aY < clipRectHeight) {
      if (maxX < clipRectWidth) {
        mPixels[aY * pitch + maxX] = aColor;
      }
      if (minX >= viewPortOffsetX) {
        mPixels[aY * pitch + minX] = aColor;
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

void BBitmap::FillCircle(
  BViewPort *aViewPort, TInt aX, TInt aY, TUint r, TUint8 aColor) {
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

  const TUint32 pitch = this->mPitch;

  // Circle is inside the viewport
  if (minX >= viewPortOffsetX && maxX < clipRectWidth &&
      minY >= viewPortOffsetY && maxY < clipRectHeight) {
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
