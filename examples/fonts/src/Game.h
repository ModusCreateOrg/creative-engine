#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include <CreativeEngine.h>
#include "Resources.h"

// Game specific includes
#include "GResources.h"
#include "GGame.h"

// GGameEngine states
#include "GGameState.h"
#include "GSplashState.h"

extern BViewPort   *gViewPort;
extern BGameEngine *gGameEngine;

#endif
