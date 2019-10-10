#ifndef BSPRITE_H
#define BSPRITE_H

#include "BList.h"
#include "BTypes.h"

class BViewPort;

class BBitmap;

class BSpriteList;

const TUint32 STYPE_DEFAULT = (0 << 0); // ordinary sprite
const TUint32 STYPE_PLAYER = (1 << 0);  // sprite of player
const TUint32 STYPE_PBULLET = (1 << 1); // sprite of player bullet
const TUint32 STYPE_ENEMY = (1 << 2);   // sprite of enemy
const TUint32 STYPE_EBULLET = (1 << 3); // sprite of enemy bullet
const TUint32 STYPE_USER_BIT = 4;       // programmer adds types starting here

const TUint32 SFLAG_CLIPPED = (1 << 0); // sprite was totally clipped
const TUint32 SFLAG_MOVE = (1 << 1);    // sprite will be moved
const TUint32 SFLAG_ANIMATE = (1 << 2); // sprite will be animated
const TUint32 SFLAG_RENDER = (1 << 3);  // sprite will be rendered
const TUint32 SFLAG_CHECK = (1 << 4);   // sprite will be checked for collisions
const TUint32 SFLAG_ANCHOR = (1 << 5);  // sprite coordinates are anchored
const TUint32 SFLAG_FLIP = (1 << 6);    // sprite will be rendered flipped
const TUint32 SFLAG_FLOP = (1 << 7);    // sprite will be rendered flopped
const TUint32 SFLAG_RIGHT = (1 << 8);   // sprite will be rendered rotated right
const TUint32 SFLAG_LEFT = (1 << 9);    // sprite will be rendered rotated left
// after moved:
const TUint32 SFLAG_SORTX = (1 << 10);   // sprite will be sorted in Y,X
const TUint32 SFLAG_SORTY = (1 << 11);   // sprite will be sorted in X,Y
const TUint32 SFLAG_SORTPRI = (1 << 12); // sprite will be sorted by priority

// programmer adds flags starting here up to 31
const TUint32 SFLAG_USER_BIT = 13;

// Permissable bit combinations for FLIP,FLOP,RIGHT,LEFT
// LEFT  RIGHT  FLOP  FLIP
//   0     0      0     0       normal
//   0     0      0     1       rotated 180 around Y axis (mirrored in X)
//   0     0      1     0       rotated 180 around X axis (mirrored in Y)
//   0     0      1     1       rotated 180 around Z axis
//   0     1      0     0       rotated  90 around Z clockwise
//   1     0      0     0       rotated  90 around Z counterclockwise
//   0     1      0     1       rotated 180 around diagonal SW-NE
//   1     0      0     1       rotated 180 around diagonal SE-NW
// For convenience these constants are for common orientations:

const TUint32 SFLAG_X180 = SFLAG_FLOP;
const TUint32 SFLAG_Y180 = SFLAG_FLIP;
const TUint32 SFLAG_Z180 = SFLAG_FLIP | SFLAG_FLOP;
const TUint32 SFLAG_Z90R = SFLAG_RIGHT;
const TUint32 SFLAG_Z90L = SFLAG_LEFT;
const TUint32 SFLAG_NORMAL = ~(SFLAG_FLIP | SFLAG_FLOP | SFLAG_RIGHT | SFLAG_LEFT); // AND to make normal

/**
* \brief BSprite is an ordinary Sprite implementation.Sprites are kept in a sorted
* linked list, sorted by priority.  This way you can control which sprites
* appear on top of others.  For example, you probably want bullets on top of
* enemies so you can see the collisions.
*
* The basic concept of a sprite is a worldX, worldY where the sprite will
* appear, and the image that will be displayed when the world's viewport is
* positioned so the sprite is visible.
*
* On top of this concept, we implement vx and vy, velocities in x and y, or
* vectored velocities.  The velocities are speciried in (world) pixels per
* frame, and are typically going to be floating point with some fractional
* component. This way you can move a sprite 1.5 pixels per frame and you get
* smooth movement over time.
*
* We also implement collision detection between sprites.  For this, we have a
* sprite "type" defined by STYPE_* constants.  You set a sprite's type in type
* and you set a mask of which other STYPE_* sprites you want to check for
* collisions against.  For example, STYPE_PLAYER will cMask STYPE_ENEMY and
* STYPE_EBULLET to check against enemies and their bullets.  Each frame, the
* cType field will contain bits set for each of the types that the sprite
* collided with.
*
* There are a rich set of Sprite flags implemented as well.  These define
* sprite state as well as rules for how the sprite is to be rendered.
* Particularly, there is SFLAG_FLIP, SFLAG_FLOP, SFLAG_LEFT, and SFLAG_RIGHT
* that render the sprite's image flipped in X, and/or flopped in Y, and/or
* rotated left 91 degrees, and/or rorated right 90 degrees.
*
* Additionally, to support platform style games, there is SFLAG_ANCHOR.  This
* specifies the Sprite's X and Y are the lower middle of the sprite instead of
* upper left corner.  This allows you to check for collisions with the ground
* where the sprite's "feet" are.
*
* Some of the flags are communication between game logic and the sprite system.
* SFLAG_CLIPPED is set by the system if the sprite was not rendered (was not
* within the viewport).  If you set SFLAG_MOVE, then the sprite's Move()
* function will be called each frame.  If you set SFLAG_ANIMATE, then the
* sprite's Animate() function will be called each frame. If you set
* SFLAG_CHECK, then the sprite WILL be checked for collisions.  You will have
* to implement your own Animate() function after inheriting from
* this class.  The default Animate() function in the base class does nothing.
* The default Move() function adds vx to x and vy to y if SFLAG_MOVE is set.
*
* See BAnimSprite, which inherits from this class and provides a feature rich
* animation engine.
*
*/
struct BSprite : public BNodePri {
public:
  BSprite(TInt aPri, TUint16 bm, TUint16 img = 0, TUint32 aType = STYPE_DEFAULT);

  BSprite(TInt aPri, TUint16 bm, TRect rect, TUint32 aType = STYPE_DEFAULT);

  ~BSprite() OVERRIDE = default;

public:
  TBool Clipped() { return TBool(flags & SFLAG_CLIPPED); }

  // shortcut inline methods to deal with flags bits
  void SetFlags(TUint32 aFlags) { flags |= aFlags; }

  void ClearFlags(TUint32 aFlags) { flags &= ~aFlags; }

  TBool TestFlags(TUint32 aFlags) { return flags & aFlags; }

  // shortcut inline methods to deal with cType bits
  void SetCType(TUint32 aFlags) { cType |= aFlags; }

  void ClearCType(TUint32 aFlags) { cType &= ~aFlags; }

  TBool TestCType(TUint32 aFlags) { return cType & aFlags; }

  // shortcut inline methods to deal with cType bits
  void SetCMask(TUint32 aFlags) { cMask |= aFlags; }

  void ClearCMask(TUint32 aFlags) { cMask &= ~aFlags; }

  TBool TestCMask(TUint32 aFlags) { return cMask & aFlags; }

public:
  virtual void GetRect(TRect &aRect); // gets collision rectangle

public:
  virtual void Move();

  virtual void Animate();

  virtual void Collide(BSprite *aOther);

  virtual TBool Render(BViewPort *aViewPort);

  static TBool
  DrawSprite(BViewPort *aViewPort, TInt16 aBitmapSlot, TInt aImageNumber, TInt aX, TInt aY, TUint32 aFlags = 0);

public:
  TUint32 type, flags;
  TUint32 cMask, cType; // for collisions
  TFloat x, y;
  TFloat vx, vy;
  TInt16 cx, cy, w, h; // collision rectangle
  TUint16 mBitmapSlot, mImageNumber;
  TRect mRect;
  BBitmap *mBitmap;
};

class BSpriteList : public BListPri {
public:
  BSpriteList();

  virtual ~BSpriteList();

  void Reset();

public:
  BSprite *RemHead() { return (BSprite *) BListPri::RemHead(); }

  BSprite *First() { return (BSprite *) next; }

  BSprite *Next(BSprite *curr) { return (BSprite *) curr->next; }

  BSprite *Last() { return (BSprite *) prev; }

  BSprite *Prev(BSprite *curr) { return (BSprite *) curr->prev; }

  TBool End(BSprite *curr) { return curr == (BSprite *) this; }

public:
  void Move();

  void Animate();

  void Render(BViewPort *aViewPort);

  void MultipleCollisions(TBool aFlag) { mMultipleCollisions = aFlag; }

  TBool MultipleCollisions() { return mMultipleCollisions; }

protected:
  TBool mMultipleCollisions;
};

// extern BSpriteList gSpriteList;

#endif
