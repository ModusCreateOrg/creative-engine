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
 * Animation Script op code bytes
 */
#define ANULLI 0
#define ABITMAPI 1
#define ASTEPI 2
#define ASTEP1I 3
#define AFLIPI 4
#define AFLIP1I 5
#define ALABELI 6
#define ALOOPI 7
#define AENDI 8
#define AFLOPI 9
#define AFLOP1I 10
#define AFLIPFLOPI 11
#define AFLIPFLOP1I 12
#define ADELTAI 13

/**
 * Animation script language (macros)
 */
#define ANULL(frames) ANULLI, (frames)
#define ABITMAP(bm) ABITMAPI, (bm)
#define ASTEP(frames, image) ASTEPI, (frames), (image)
#define ASTEP1(image) ASTEP1I, (image)
#define AFLIP(frames, image) AFLIPI, (frames), (image)
#define AFLIP1(image) AFLIP1I, (image)
#define AFLOP(frames, image) AFLOPI, (frames), (image)
#define AFLOP1(image) AFLOP1I, (image)
#define AFLIPFLOP(frames, image) AFLIPFLOPI, (frames), (image)
#define AFLIPFLOP1(image) AFLIPFLOP1I, (image)
#define ALABEL ALABELI
#define ALOOP ALOOPI
#define AEND AENDI
#define ADELTA(dx,dy) ADELTAI, (dx), (dy)

#endif
