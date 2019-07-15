#include <cmath>
#include "BSprite.h"
#include "BViewPort.h"
#include "BResourceManager.h"
#include "Display/Display.h"

BSpriteList gSpriteList;

BSprite::BSprite(TInt aPri, TUint16 bm, TUint16 img, TUint32 aType)
  : BNodePri(aPri) {
  flags = SFLAG_RENDER | SFLAG_MOVE | SFLAG_ANIMATE;
  type = aType;
  cMask = cType = 0;
  x = y = 0;
  w = 16;
  h = 16; // assume 16x16, let owner change these
  vx = vy = 0;
  mBitmapSlot = bm;
  mImageNumber = img;
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
  cMask = cType = 0;
  x = y = 0;
  w = TUint16(rect.Width());
  h = TUint16(rect.Height());
  vx = vy = 0;
  mBitmapSlot = bm;
  mImageNumber = 0;
  mRect = rect;
  mBitmap = gResourceManager.GetBitmap(bm);
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
    TInt dy = gResourceManager.BitmapHeight(mBitmapSlot) / 2;
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

    mRect.x1 = (mImageNumber % pitch) * bw;
    mRect.x2 = mRect.x1 + bw - 1;
    mRect.y1 = (mImageNumber / pitch) * bh;
    mRect.y2 = mRect.y1 + bh - 1;

    return (mBitmap->TransparentColor() != -1)
           ? gDisplay.renderBitmap->DrawBitmapTransparent(aViewPort, mBitmap, mRect, round(screenX), round(screenY),
                                                          (flags >> 6) & 0x0f)
           : gDisplay.renderBitmap->DrawBitmap(aViewPort, mBitmap, mRect, round(screenX), round(screenY),
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
    switch (flags & ~SFLAG_NORMAL) { // compute upper-left corner
      case 0:
      case SFLAG_FLIP:
        yy = TInt(y) - h;
        xx = TInt(x) - (w / 2);
        break;
      case SFLAG_FLOP:
      case SFLAG_FLIP | SFLAG_FLOP:
        yy = TInt(y);
        xx = TInt(x) - (w / 2);
        break;
      case SFLAG_RIGHT:
      case SFLAG_FLIP | SFLAG_RIGHT:
        yy = TInt(y) - (h / 2);
        xx = TInt(x);
        break;
      case SFLAG_LEFT:
      case SFLAG_FLIP | SFLAG_LEFT:
        yy = TInt(y) - (h / 2);
        xx = TInt(x) - w;
        break;
    }
  } else {
    xx = TInt(x);
    yy = TInt(y);
  }
  aRect.Set(xx, yy, xx + w - 1, yy + h - 1);
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

    if (s->flags & SFLAG_CHECK) {
      // check for collisions
      s->GetRect(myRect); // my rectangle for collisions
      // check collisions with objects behind me on the list
      for (s2 = First(); s2 != s; s2 = Next(s2)) {
        // make sure these two sprites care about collisions vs. each other
        if (!(s->cMask & s2->type))
          continue;

        // check collision rectangles
        s2->GetRect(hisRect);
        if (myRect.Overlaps(hisRect)) { // COLLISION!
          // don't record collision if s already collided with s2
          if (mMultipleCollisions || !(s->cType & s2->type)) {
            s->Collide(s2);
          }

          // don't record collision if:
          // s2 is not checking for collisions
          // s2 doesn't care about s
          // or s2 already collided with s
          if ((s2->flags & SFLAG_CHECK) &&                        // s2 checks collisions
              (s2->cMask & s->type) &&                            // s2 cares about colliding with s
              (mMultipleCollisions ||
               !(s2->cType & s->type))) {  // multiple collisions allowed or s2 hasn't collided with s
            s2->Collide(s);
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
    if (s->flags & SFLAG_SORTPRI) {
      while (1) { // sort back in Pri
        s2 = Prev(s);
        if (End(s2)) {
          break; // stop if we are first in list
        }
        if (s->pri > s2->pri) {
          break;
        }
        s->Remove(); // move back in list
        s->InsertBeforeNode(s2);
      }
    }
    // next sprite
    s = sn;
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
