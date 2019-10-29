#ifdef __MODUS_TARGET_DESKTOP_DISPLAY__

#include "DesktopDisplay.h"

#include <SDL.h>

static SDL_Window *screen = ENull;
static SDL_Renderer *renderer = ENull;
static SDL_Texture *texture = ENull;

DesktopDisplay::DesktopDisplay() : DisplayBase() {

  SDL_Init(SDL_INIT_VIDEO); // Initialize SDL2

  int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN;

#ifdef CE_SDL2_WINDOW_TITLE
  const char *windowTitle = CE_SDL2_WINDOW_TITLE; // window title
#else
  const char *windowTitle = "creative-engine"; // window title
#endif

  screen = SDL_CreateWindow(
    windowTitle,
    SDL_WINDOWPOS_UNDEFINED, // initial resources position
    SDL_WINDOWPOS_UNDEFINED, // initial y position
    SCREEN_WIDTH * 4,        // Width in pixels
    SCREEN_HEIGHT * 4,       // Height in pixels
    flags                    // flags - see above
  );

  SDL_SetWindowMinimumSize(screen, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2);

  if (screen == ENull) {
    printf("Could not create window: %s\n", SDL_GetError());
    exit(1);
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
    printf("Cannot create texture %s\n", SDL_GetError());
  }

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  SDL_GL_SetSwapInterval(1);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  // try to move window, to fix SDL2 bug on MacOS (Mojave)
  int x, y;
  SDL_GetWindowPosition(screen, &x, &y);
  SDL_SetWindowPosition(screen, x + 1, y + 1);
}

DesktopDisplay::~DesktopDisplay() {
  // Close and destroy the window
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(screen);

  // Clean up
  SDL_Quit();
}

static TBool hackInitialized = EFalse;

void DesktopDisplay::Update() {
  // try to move window, to fix SDL2 bug on MacOS (Mojave)
  if (!hackInitialized) {
    int x, y;
    SDL_GetWindowPosition(screen, &x, &y);
    SDL_SetWindowPosition(screen, x + 1, y + 1);
    SDL_SetWindowPosition(screen, x, y);
    hackInitialized = ETrue;
  }
  // swap display and render bitmaps
  SwapBuffers();

  void *screenBuf;
  TInt pitch;

  if (0 == SDL_LockTexture(texture, ENull, &screenBuf, &pitch)) {
	TUint32 colors[256];
    auto *screenBits = (TUint32 *) screenBuf;
    TRGB *palette = displayBitmap->GetPalette();

    for (TInt c = 0; c < 256; c++) {
      colors[c] = palette[c].rgb888();
    }

    for (TInt y = 0; y < SCREEN_HEIGHT; y++) {
      TUint8 *ptr = &displayBitmap->mPixels[y * displayBitmap->GetPitch()];

      for (TInt x = 0; x < SCREEN_WIDTH; x++) {
        TUint8 pixel = *ptr++;
        *screenBits++ = colors[pixel];
      }
    }

    SDL_UnlockTexture(texture);
  } else {
    printf("Can't lock texture (%s)\n", SDL_GetError());
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, ENull, ENull); // Render texture to entire window
  SDL_RenderPresent(renderer);                     // Do update

  NextFrameDelay();
}

#endif
