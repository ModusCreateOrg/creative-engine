//
// Created by mschwartz on 8/25/18.
//

#include "GameEngine.h"

#ifndef __XTENSA__

#include "SDL2/SDL.h"

#endif

#ifndef __XTENSA__
static SDL_Window   *screen   = nullptr;
static SDL_Renderer *renderer = nullptr;
static SDL_Texture  *texture  = nullptr;
#endif

Display display;

Display::Display() {
  // initialize any hardware
#ifndef __XTENSA__

  SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2

  // Create an application window with the following settings:
  screen = SDL_CreateWindow(
    "An SDL2 window",                  // window title
    SDL_WINDOWPOS_UNDEFINED,           // initial x position
    SDL_WINDOWPOS_UNDEFINED,           // initial y position
    SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2,   // width, in pixels
    SDL_WINDOW_OPENGL                  // flags - see below
  );

  // Check that the window was successfully created
  if (screen == nullptr) {
    // In the case that the window could not be made...
    printf("Could not create window: %s\n", SDL_GetError());
    exit(1);
  } else {
    int w, h;
    SDL_GL_GetDrawableSize(screen, &w, &h);
    printf("widthxheight: %dx%d\n", w, h);
  }

  renderer = SDL_CreateRenderer(screen, -1, 0);
  if (!renderer) {
    printf("Cannot create renderer %s\n", SDL_GetError());
    exit(1);
  }
  SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
  SDL_RenderSetIntegerScale(renderer, SDL_TRUE);

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH,
                              SCREEN_HEIGHT);
  if (!texture) {
    printf("Cannot create texturre %s\n", SDL_GetError());
  }
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  SDL_GL_SetSwapInterval(1);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

#endif
  mBitmap1      = new BBitmap(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH);
  mBitmap2      = new BBitmap(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH);
  renderBitmap  = mBitmap1;
  displayBitmap = mBitmap2;
}

Display::~Display() {
  // clean up
#ifndef __XTENSA__
  // Close and destroy the window
  SDL_DestroyWindow(screen);

  // Clean up
  SDL_Quit();
  delete mBitmap1;
  delete mBitmap2;
  SDL_DestroyWindow(screen);
  SDL_Quit();
#endif
}

void Display::Update() {
  // swap display and render bitmaps
  if (renderBitmap == mBitmap1) {
    renderBitmap  = mBitmap2;
    displayBitmap = mBitmap1;
  } else {
    renderBitmap  = mBitmap1;
    displayBitmap = mBitmap2;
  }

#ifndef __XTENSA__
  void *screenBuf;
  TInt    pitch;
  int ret;
  if (0 == SDL_LockTexture(texture, nullptr, &screenBuf, &pitch)) {
    auto *screenBits = (TUint32 *)screenBuf;
    TRGB *palette = displayBitmap->mPalette;
    for (TInt16 y = 0; y < SCREEN_HEIGHT; y++) {
      TUint8    *ptr = &displayBitmap->mPixels[y * displayBitmap->mPitch];
      for (TInt x    = 0; x < SCREEN_WIDTH; x++) {
        TUint8 pixel = *ptr++;
        TUint32 color = palette[pixel].rgb888();
        *screenBits++ = color;
      }
    }
    screenBits = (TUint32 *)screenBuf;
//    Dump(screenBits, renderBitmap->mWidth);
//    Dump(displayBitmap->mPixels, displayBitmap->mWidth, displayBitmap->mHeight);
    SDL_UnlockTexture(texture);
  }
  else {
    printf("Can't lock texture (%s)\n", SDL_GetError());
  }
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, nullptr, nullptr); // Render texture to entire window
  SDL_RenderPresent(renderer);              // Do update
#endif
}
