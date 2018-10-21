//
// Created by Michael Schwartz on 9/5/18.
//

#ifndef GAME_ENGINE_BPLAYFIELD_H
#define GAME_ENGINE_BPLAYFIELD_H

#include "BBase.h"

class BGameEngine;
class Display;

/**
 * Abstract BPlayfield class.
 *
 * Inherit from this.  Implement a Render() function that renders the current state of the backtground/playfield
 * to the Display's renderBitmap.
 *
 * Sprites will be rendered on top
 */
class BPlayfield : public BBase {
public:
  BPlayfield();

  virtual ~BPlayfield();

  virtual void Animate();

  virtual void Render() = 0;
};


#endif //GAME_ENGINE_BPLAYFIELD_H
