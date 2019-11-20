#include <cmath>
#include "BSprite.h"
#include "BSpriteSheet.h"
#include "BViewPort.h"
#include "BResourceManager.h"
#include "Display/Display.h"
#include <assert.h>

// BSpriteList gSpriteList;

BSprite::BSprite(TInt aPri, TUint16 bm, TUint16 img, TUint32 aType)
  : BNodePri(aPri) {
  flags = SFLAG_RENDER | SFLAG_MOVE | SFLAG_ANIMATE;
  type = aType;
  mSignals = cMask = cType = 0;
  x = y = 0;
  cx = 0;
  cy = 0;
  w = 16;
  h = 16; // assume 16x16, let owner change these
  vx = vy = 0;
  mBitmapSlot = bm;
  mImageNumber = img;
  mSpriteSheet = ENull; // if this is set, we'll use the rect info in the Sprite Sheet to render the sprite.
  mBitmap = gResourceManager.GetBitmap(bm);
  TInt bw = gResourceManager.BitmapWidth(bm),
    bh = gResourceManager.BitmapHeight(bm),
    pitch = mBitmap->Width() / bw;

  mRect.x1 = (img % pitch) * bw;
  mRect.x2 = mRect.x1 + bw - 1;
  mRect.y1 = (img / pitch) * bh;
  mRect.y2 = mRect.y1 + bh - 1;
}

BSprite::BSprite(TInt aPri, TUint16 bm, TRect rect, TUint32 aType)
  : BNodePri(aPri) {
  flags = SFLAG_RENDER | SFLAG_MOVE | SFLAG_ANIMATE;
  type = aType;
  mSignals = cMask = cType = 0;
  x = y = 0;
  w = TUint16(rect.Width());
  h = TUint16(rect.Height());
  vx = vy = 0;
  mBitmapSlot = bm;
  mImageNumber = 0;
  mRect = rect;
  mBitmap = gResourceManager.GetBitmap(bm);
}

BSprite::~BSprite() {
  if (mSpriteSheet) {
    delete mSpriteSheet;
    mSpriteSheet = ENull;
  }
}

void BSprite::Move() {
  if (flags & SFLAG_MOVE) {
    x += vx;
    y += vy;
  }
}

void BSprite::Animate() {}

TBool BSprite::Render(BViewPort *aViewPort) {
  TFloat screenX = x - aViewPort->mWorldX;
  TFloat screenY = y - aViewPort->mWorldY;
  if (flags & SFLAG_ANCHOR) {
    TInt dy = gResourceManager.BitmapHeight(mBitmapSlot);
    if (flags & SFLAG_FLOP) {
      screenY += dy;
    } else {
      screenY -= dy;
    }
  }

  if (flags & SFLAG_RENDER) {
    mBitmap = gResourceManager.GetBitmap(mBitmapSlot);
    TInt bw = gResourceManager.BitmapWidth(mBitmapSlot),
      bh = gResourceManager.BitmapHeight(mBitmapSlot),
      pitch = mBitmap->Width() / bw;

    mRect.x1 = TInt32(screenX);
    mRect.y1 = TInt32(screenY);
    mRect.x2 = TInt32(screenX) + gResourceManager.BitmapWidth(mBitmapSlot) - 1;
    mRect.y2 = TInt32(screenY) + gResourceManager.BitmapHeight(mBitmapSlot) - 1;

    TRect srcRect;
    if (mSpriteSheet) {
      BSpriteInfo *info = mSpriteSheet->GetSpriteInfo(mImageNumber);
      srcRect.x1 = info->x1;
      srcRect.y1 = info->y1;
      srcRect.x2 = info->x2;
      srcRect.y2 = info->y2;
      screenX += info->dx;
      screenY += info->dy;
      if (srcRect.x1 > 1024) {
        srcRect.Dump();
      }
    } else {
      srcRect.x1 = (mImageNumber % pitch) * bw;
      srcRect.x2 = srcRect.x1 + bw - 1;
      srcRect.y1 = (mImageNumber / pitch) * bh;
      srcRect.y2 = srcRect.y1 + bh - 1;
    }

    return (mBitmap->TransparentColor() != -1)
           ? gDisplay.renderBitmap->DrawBitmapTransparent(aViewPort, mBitmap, srcRect, round(screenX), round(screenY),
                                                          (flags >> 6) & 0x0f)
           : gDisplay.renderBitmap->DrawBitmap(aViewPort, mBitmap, srcRect, round(screenX), round(screenY),
                                               (flags >> 6) & 0x0f);
  }

  return ETrue;
}

TBool
BSprite::DrawSprite(BViewPort *aViewPort, TInt16 aBitmapSlot, TInt aImageNumber, TInt aX, TInt aY, TUint32 aFlags) {
  BBitmap *b = gResourceManager.GetBitmap(aBitmapSlot);
  TInt bw = gResourceManager.BitmapWidth(aBitmapSlot),
    bh = gResourceManager.BitmapHeight(aBitmapSlot),
    pitch = b->Width() / bw;

  TRect imageRect;
  imageRect.x1 = (aImageNumber % pitch) * bw;
  imageRect.x2 = imageRect.x1 + bw - 1;
  imageRect.y1 = (aImageNumber / pitch) * bh;
  imageRect.y2 = imageRect.y1 + bh - 1;

  return b->TransparentColor()
         ? gDisplay.renderBitmap->DrawBitmapTransparent(aViewPort, b, imageRect, aX, aY, (aFlags >> 6) & 0x0f)
         : gDisplay.renderBitmap->DrawBitmap(aViewPort, b, imageRect, aX, aY, (aFlags >> 6) & 0x0f);
}

void BSprite::Collide(BSprite *aOther) { cType |= aOther->type; }

void BSprite::GetRect(TRect &aRect) {
  TInt xx = 0, yy = 0;
  if (flags & SFLAG_ANCHOR) {
    xx = TInt(x + w / 2);
    yy = TInt(y - h);
    //    switch (flags & ~SFLAG_NORMAL) { // compute upper-left corner
    //      case 0:
    //      case SFLAG_FLIP:
    //        yy = TInt(y) - h;
    //        xx = TInt(x) - (w / 2);
    //        break;
    //      case SFLAG_FLOP:
    //      case SFLAG_FLIP | SFLAG_FLOP:
    //        yy = TInt(y);
    //        xx = TInt(x) - (w / 2);
    //        break;
    //      case SFLAG_RIGHT:
    //      case SFLAG_FLIP | SFLAG_RIGHT:
    //        yy = TInt(y) - (h / 2);
    //        xx = TInt(x);
    //        break;
    //      case SFLAG_LEFT:
    //      case SFLAG_FLIP | SFLAG_LEFT:
    //        yy = TInt(y) - (h / 2);
    //        xx = TInt(x) - w;
    //        break;
    //    }
  } else {
    xx = TInt(x);
    yy = TInt(y);
  }
  aRect.Set(cx + xx, cy + yy, cx + xx + w - 1, cy + yy + h - 1);
}

BSpriteList::BSpriteList() : BListPri() { mMultipleCollisions = EFalse; }

BSpriteList::~BSpriteList() {
  //
  Reset();
}

void BSpriteList::Reset() {
  while (BSprite *s = RemHead()) {
    delete s;
  }
}

void BSpriteList::Move() {
  BSprite *s;
  BSprite *s2;
  BSprite *sn;
  TRect myRect;
  TRect hisRect;

  s = First();
  while (!End(s)) {
    sn = Next(s);

    // move sprite
    s->Move();

    // check for collisions
    if (s->flags & SFLAG_CHECK) {
      s->GetRect(myRect); // my rectangle for collisions
      // check collisions with objects behind me on the list
      for (s2 = First(); s2 != s; s2 = Next(s2)) {
        // make sure these two sprites care about collisions vs. each other
        if (s2->flags & SFLAG_CHECK) {
          if (!(s->cMask & s2->type)) {
            continue;
          }
          if (!(s2->cMask & s->type)) {
            continue;
          }
          if (mMultipleCollisions || (!(s->cType & s2->type) && !(s2->cType & s->type))) {
            // check collision rectangles
            s2->GetRect(hisRect);
            if (myRect.Overlaps(hisRect)) {
              s->Collide(s2);
              s2->Collide(s);
            }
          }
        }
      }
    }

    // sort back in list
    if (s->flags & SFLAG_SORTY) {
      while (1) { // sort back in Y
        s2 = Prev(s);
        if (End(s2))
          break; // stop if we are first in list
        if (s->y > s2->y)
          break;
        if (s->y == s2->y && s->x > s2->x)
          break;
        s->Remove(); // move back in list
        s->InsertBeforeNode(s2);
      }
    }
    if (s->flags & SFLAG_SORTX) {
      while (1) { // sort back in X
        s2 = Prev(s);
        if (End(s2)) {
          break; // stop if we are first in list
        }
        if (s->x > s2->x) {
          break;
        }
        if (s->x == s2->x && s->y > s2->y) {
          break;
        }
        s->Remove(); // move back in list
        s->InsertBeforeNode(s2);
      }
    }
    if (s->TestFlags(SFLAG_SORTPRI)) {
      for (s2 = Prev(s); !End(s2); s2 = Prev(s2)) {
        if (s2->TestFlags(SFLAG_SORTPRI)) {
          // if two sprites with the same pri are getting swapped every frame, make this > instead of >=
          if (s->pri >= s2->pri) {
            break;
          }
        }

      }
      s->Remove(); // move back in list
      s->InsertAfterNode(s2);
    }

    // next sprite
    s = sn;
  }

  // TODO: do we want to assert and crash in production?
  assert(ChkPriOrder());
}

void BSpriteList::Dump() {
  for (auto *s = First(); !End(s); s = Next(s)) {
    printf("Sprite %p x,y %.0f,%.0f flags %x pri: %d SORT: %s\n", s, s->x, s->y, s->flags, s->pri,
           s->TestFlags(SFLAG_SORTPRI) ? "SORT" : "NOSORT");
  }
}

TBool BSpriteList::ChkPriOrder() {
  TInt v = First()->pri;
  for (auto *s = First(); !End(s); s = Next(s)) {
    if (s->TestFlags(SFLAG_SORTPRI)) {
      if (s->pri >= v) {
        v = s->pri;
      } else {
        printf("\n\nASSERT DUMP\n");
        Dump();
        return EFalse;
      }
    }
  }
  return ETrue;
}

void BSpriteList::Signal(TUint32 aSignal) {
  for (BSprite *s = First(); !End(s); s = Next(s)) {
    s->mSignals |= aSignal;
  }
}

void BSpriteList::Animate() {
  for (BSprite *s = First(); !End(s); s = Next(s)) {
    s->Animate();
  }
}

void BSpriteList::Render(BViewPort *aViewPort) {
  for (BSprite *s = First(); !End(s); s = Next(s)) {
    if (s->Render(aViewPort)) {
      s->flags &= ~(SFLAG_CLIPPED); // render and determine if clipped
    } else {
      s->flags |= SFLAG_CLIPPED;
    }
  }
}
