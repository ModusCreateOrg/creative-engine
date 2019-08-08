#ifndef BANIMSPRITE_H
#define BANIMSPRITE_H

#include "BSprite.h"

typedef const TInt16 ANIMSCRIPT;

/**
 * BAnimSprite is an enhanced BSprite that provides an animation interpreter so you can script
 * animations for your sprites.
 *
 * Animation scripts are simply an array of bytes.  The interpreter reads bytes,
 * in order, from the script.  A byte is interpreted as an op code that defines
 * what to do this game loop.  Op codes can be followed by additional data that
 * are parameters for the op code.  For example an op code to set the displayed
 * image will be followed by a byte that is the image id as well as a byte that
 * specifies the number of game loops (frames) that the image is to be displayed
 * and when the next op code will be processed.
 *
 * Important to note that op codes are executed periodically - when the
 * animation timer hits zero, the next op code is executed.  Over time, an
 * animation is produced.
 *
 * Game logic (typically done in a BProcess) will call StartAnimation() to set
 * the program to be run.  The logic might then wait for the AnimDone()
 * animation to be finished and then start a new program.
 *
 * Example game logic, to control a walking player:
 * 1) When there is no joystick input, start and run the standing still
 * animation.
 *
 * 2) When the joystick is right, start the run right animation.
 *
 * 3) When the joystick is left, start the run left animation.
 *
 * 4) When there is no floor under the player, start the fall animation.
 */
class BAnimSprite : public BSprite {
public:
  BAnimSprite(TInt aPri, TUint16 bm, TUint16 img = 0,
              TUint16 aType = STYPE_DEFAULT);
  virtual ~BAnimSprite();
  virtual void Animate();

public:
  void StartAnimation(ANIMSCRIPT aScript[]);
  TBool AnimDone() { return (flags & SFLAG_ANIMATE) ? EFalse : ETrue; }
public:
  // override Render to consider mDx,mDy
  TBool Render(BViewPort *aViewPort);
public:
  TFloat mDx, mDy;
protected:
  ANIMSCRIPT *mAnimPtr;
  TInt32 mAnimTimer;
  TInt32 mAnimIndex;
  TInt32 mAnimLoop;
};

/**
 * Animation Script op code (instruction) bytes
 */
const TInt16 ANULLI = 0;
const TInt16 ABITMAPI = 1;
const TInt16 ASTEPI = 2;
const TInt16 ASTEP1I = 3;
const TInt16 AFLIPI = 4;
const TInt16 AFLIP1I = 5;
const TInt16 ALABELI = 6;
const TInt16 ALOOPI = 7;
const TInt16 AENDI = 8;
const TInt16 AFLOPI = 9;
const TInt16 AFLOP1I = 10;
const TInt16 AFLIPFLOPI = 11;
const TInt16 AFLIPFLOP1I = 12;
const TInt16 ADELTAI = 13;
const TInt16 ATYPEI = 14;
const TInt16 ASIZEI = 15;

/**
 * Animation script language (macros)
 */

// display NO image at all for #frames
#define ANULL(frames) ANULLI, (frames)

// the bitmap slot to render image from
#define ABITMAP(bm) ABITMAPI, (bm)

// show image # for number of frames
#define ASTEP(frames, image) ASTEPI, (frames), (image)

// show image # for 1 frame
#define ASTEP1(image) ASTEP1I, (image)

// show image # FLIPPED for number of frames
#define AFLIP(frames, image) AFLIPI, (frames), (image)

// show image # FLIPPED for 1 frame
#define AFLIP1(image) AFLIP1I, (image)

// show image # FLOPPED for number of frames
#define AFLOP(frames, image) AFLOPI, (frames), (image)

// show image # FLOPPED for 1 frame
#define AFLOP1(image) AFLOP1I, (image)

// show image # FLIPPED AND FLOPPED for number of frames
#define AFLIPFLOP(frames, image) AFLIPFLOPI, (frames), (image)

// show image # FLIPPED AND FLOPPED for 1 frame
#define AFLIPFLOP1(image) AFLIPFLOP1I, (image)

// set loop starting point (only one allowed per script)
#define ALABEL ALABELI

// go to ALABEL
#define ALOOP ALOOPI

// END of animation, clear SFLAG_ANIMATE
#define AEND AENDI

// set mDX and mDY to offset rendering of image
#define ADELTA(dx,dy) ADELTAI, (dx), (dy)

// set type for collisions
#define ATYPE(type) ATYPEI, (type)

// set rect for collisions
#define ASIZE(w, h) ASIZEI, (w), (h)

#endif
