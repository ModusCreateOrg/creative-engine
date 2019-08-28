#ifdef __DINGUX__

#ifndef MODITE_LDKDISPLAY_H
#define MODITE_LDKDISPLAY_H


#include <SDL/SDL.h>
#include <iostream>
#include <ctime>
#include <cstdlib>

#include "DisplayDefines.h"
#include "DisplayBase.h"



class LDKDisplay : public DisplayBase {
public:
  LDKDisplay() {
    SDL_ShowCursor(SDL_DISABLE);

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == -1) {
      printf("Could not load SDL : %s\n", SDL_GetError());
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

    mDrawSurface = SDL_CreateRGBSurface(
      SDL_HWSURFACE,
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
      8,
      0,
      0,
      0,
      0
    );
  }

  void Init() override {};

//  uint32_t color = 0;
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;

  void Update() override {

    SDL_Rect destination;
    destination.x = 0;
    destination.y = 0;


    void *screenBuf;
    TInt pitch;

    SwapBuffers();


    auto *screenBits = (TUint16 *) mSDLScreen->pixels;
    TRGB *palette    = displayBitmap->GetPalette();

    TInt32 z = 0;
    for (TInt16 y = 0; y < SCREEN_HEIGHT; y++) {
      TUint8 *ptr = &displayBitmap->mPixels[y * displayBitmap->GetPitch()];

      for (TInt16 x = 0; x < SCREEN_WIDTH; x++) {
        TUint8  pixel = *ptr++;
        *screenBits++ = palette[pixel].rgb565();
      }

    }


    if (SDL_MUSTLOCK(mSDLScreen)) SDL_UnlockSurface(mSDLScreen);
    SDL_Flip(mSDLScreen);
    if (SDL_MUSTLOCK(mSDLScreen)) SDL_LockSurface(mSDLScreen);

    NextFrameDelay();


  }

  ~LDKDisplay() {
    SDL_FreeSurface(mDrawSurface);
    SDL_FreeSurface(mSDLScreen);
    SDL_Quit();
  }

  SDL_Surface *mDrawSurface;
  SDL_Surface *mSDLScreen;

  TUint16 color565(TUint8 red, TUint8 green, TUint8 blue) override{
    // lifted from Display2.cpp
    // maybe we want to return something else for SDL
    return ((blue & 0xF8) << 8) | ((red & 0xFC) << 3) | ((green & 0xF8) >> 3);
  }

};



#endif //MODITE_LDKDISPLAY_H

#endif