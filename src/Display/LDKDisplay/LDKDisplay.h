#ifdef __DINGUX__

#ifndef MODITE_LDKDISPLAY_H
#define MODITE_LDKDISPLAY_H


#include <SDL/SDL.h>
#include <iostream>
#include <ctime>
#include <cstdlib>

#include "DisplayBase.h"
#include "DisplayDefines.h"



class LDKDisplay : public DisplayBase {
public:
  LDKDisplay() {
    SDL_ShowCursor(SDL_DISABLE);

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) == -1) {
      fprintf(stderr, "Could Initiate SDL! %s\n", SDL_GetError());
      fprintf(stderr, "EXITING...");
      exit(-1);
    }

    atexit(SDL_Quit);

    mSDLScreen = SDL_SetVideoMode(
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
      16,
      SDL_HWSURFACE |

      #ifdef SDL_TRIPLEBUF
      SDL_TRIPLEBUF
      #else
      SDL_DOUBLEBUF
      #endif
    );


  }

  void Init() override {};

  void Update() override {
    SwapBuffers();

	TUint16 colors[256];
    auto *screenPixels = (TInt16 *) mSDLScreen->pixels;
    TRGB *palette = displayBitmap->GetPalette();

	for (TInt c = 0; c < 256; c++) {
      colors[c] = palette[c].rgb565();
    }

    for (TInt y = 0; y < SCREEN_HEIGHT; y++) {
      TUint8 *src = &displayBitmap->mPixels[y * displayBitmap->GetPitch()];

      for (TInt x = 0; x < SCREEN_WIDTH; x++) {
        TUint8 pixel = *src++;
        *screenPixels++ = colors[pixel];
      }
    }

    if (SDL_MUSTLOCK(mSDLScreen)) SDL_UnlockSurface(mSDLScreen);
    SDL_Flip(mSDLScreen);
    if (SDL_MUSTLOCK(mSDLScreen)) SDL_LockSurface(mSDLScreen);

    NextFrameDelay();
  }

  ~LDKDisplay() {
    SDL_FreeSurface(mSDLScreen);
    SDL_Quit();
  }

  SDL_Surface *mSDLScreen;

  TUint16 color565(TUint8 red, TUint8 green, TUint8 blue) override{
    // lifted from Display2.cpp
    // maybe we want to return something else for SDL
    return ((blue & 0xF8) << 8) | ((red & 0xFC) << 3) | ((green & 0xF8) >> 3);
  }

};



#endif //MODITE_LDKDISPLAY_H

#endif
