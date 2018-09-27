#include "BSprite.h"
#include "BViewPort.h"
#include "BResourceManager.h"
#include "Display.h"

BSprite::BSprite(TInt aPri, TUint16 bm, TUint16 img, TUint32 aType)
  : BNodePri(aPri) {
  flags        = SFLAG_RENDER | SFLAG_MOVE | SFLAG_ANIMATE;
  type         = aType;
  cMask        = cType = 0;
  x            = y     = 0;
  w            = 16;
  h            = 16; // assume 16x16, let owner change these
  vx           = vy    = 0;
  mBitmapNumber   = bm;
  mImageNumber = img;
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
    TInt dy = gResourceManager.BitmapHeight(mBitmapNumber) / 2;
    if (flags & SFLAG_FLOP) {
      screenY += dy;
    } else {
      screenY -= dy;
    }
  }
  if (flags & SFLAG_RENDER) {
    return display.renderBitmap->DrawSprite(aViewPort, mBitmapNumber, mImageNumber, screenX, screenY, flags);
  } else {
    return ETrue;
  }
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
    xx = TInt(x) - (w / 2);
    yy = TInt(y) - (h / 2);
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
  TRect   myRect;
  TRect   hisRect;

  s = First();
  while (!End(s)) {
    sn = Next(s);

    // move sprite
    s->Move();

    // check for collisions
    s->GetRect(myRect); // my rectangle for collisions
    // check collisions with objects behind me on the list
    for (s2 = First(); s2 != s; s2 = Next(s2)) {
      // make sure these two sprites care about collisions vs. each other
      if (!(s->cMask & s2->type))
        continue;
      if (!(s2->cMask & s->type))
        continue;

      // check collision rectangles
      s2->GetRect(hisRect);
      if (myRect.Overlaps(hisRect)) { // COLLISION!
        // don't record collision if s or s2 already collided
        if (mMultipleCollisions ||
            (!(s->cType & s2->type) && !(s2->cType & s->type))) {
          s->Collide(s2);
          s2->Collide(s);
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
    s                              = sn;
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
