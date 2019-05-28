#ifdef __MODUS_TARGET_NETWORK_DISPLAY__

#include "NetworkDisplay.h"


#include "Display.h"
#include <unistd.h>
#include <stdio.h>


#include "SDL2/SDL.h"

static SDL_Window   *screen   = ENull;
static SDL_Renderer *renderer = ENull;
static SDL_Texture  *texture  = ENull;
static TBool hackInitialized = EFalse;

#include "RemoteMatrixSegment.h"
#include "RemoteMatrixDisplay.h"

#define __USE_SDL_VIDEO__ 1
#undef __USE_SDL_VIDEO__

NetworkDisplay::NetworkDisplay() : DisplayBase() {

  // initialize any hardware
#ifdef __USE_SDL_VIDEO__
  SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2

  int flags =  SDL_WINDOW_OPENGL |  SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_RESIZABLE| SDL_WINDOW_SHOWN;

  // Create an application window with the following settings:
  screen = SDL_CreateWindow(
    "creative-engine",                  // window title
    SDL_WINDOWPOS_UNDEFINED,           // initial resources position
    SDL_WINDOWPOS_UNDEFINED,           // initial y position
    SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2,   // width, in pixels
    flags                        // flags - see below
  );

  SDL_SetWindowMinimumSize(screen, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2);

  // Check that the window was successfully created
  if (screen == ENull) {
    // In the case that the window could not be made...
    printf("Could not create window: %s\n", SDL_GetError());
    exit(1);
  }

  renderer = SDL_CreateRenderer(screen, -1, 0);
  if (! renderer) {
    printf("Cannot create renderer %s\n", SDL_GetError());
    exit(1);
  }

  SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
  SDL_RenderSetIntegerScale(renderer, SDL_TRUE);

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH,
                              SCREEN_HEIGHT);
  if (! texture) {
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
  SDL_SetWindowPosition(screen, x+1, y+1);
#else
//  SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO);              // Initialize SDL2
#endif
}




RemoteMatrixDisplay *matrixDisplay;

void NetworkDisplay::Init() {
  RemoteMatrixDisplayConfig displayConfig;

  displayConfig.inputScreenWidth = SCREEN_WIDTH;
  displayConfig.inputScreenHeight = SCREEN_HEIGHT;

  // 5 matrix columns. Each column is FOUR matrices tall.
  displayConfig.singlePanelWidth = 64;
  displayConfig.singlePanelHeight = 64;
  displayConfig.totalPanelsWide = 5;
  displayConfig.totalPanelsTall = 3;

  displayConfig.outputScreenWidth = displayConfig.totalPanelsWide * displayConfig.singlePanelWidth;
  displayConfig.outputScreenHeight= displayConfig.totalPanelsTall * displayConfig.singlePanelHeight;


#define __USE_LOCAL_IP_
#undef __USE_LOCAL_IP__

#ifdef __USE_LOCAL_IP__
  char *ipRoot = "127.0.0.1";
  char *portRoot = "989%i";
#else
  char *portRoot = "989%i";
  char *ipRoot = "10.0.1.20%i";

#endif

  int ipFinalDigit = 1;
  int portFinalDigit = 0;

  std::vector<RemoteMatrixSegmentConfig> segments = displayConfig.segments;

  for (TUint8 i = 0; i < 5; i++) {
    RemoteMatrixSegmentConfig segmentConfig;

    segmentConfig.segmentId = i;
    segmentConfig.singlePanelHeight = displayConfig.singlePanelHeight;
    segmentConfig.singlePanelWidth = displayConfig.singlePanelWidth;

    segmentConfig.numPanelsWide = 1;
    segmentConfig.numPanelsTall = 3;


#ifdef __USE_LOCAL_IP__
    char *destinationIp = (char *)malloc(strlen("127.0.0.1"));
    sprintf(destinationIp, "127.0.0.1", "");
    char *destinationPort= (char *)malloc(strlen(portRoot));
    sprintf(destinationPort, portRoot, portFinalDigit++);
    segmentConfig.destinationPort = destinationPort;
#else
    char *destinationIp = (char *)malloc(strlen(ipRoot));
    sprintf(destinationIp, ipRoot, ipFinalDigit++);
    segmentConfig.destinationPort = "9890";
#endif

    segmentConfig.destinationIP = destinationIp;

    segments.push_back(segmentConfig);

  }

  displayConfig.segments = segments;
  displayConfig.inputBufferSize = SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(TUint16);

  matrixDisplay = new RemoteMatrixDisplay(displayConfig);
  matrixDisplay->StartThread();
}

//static TBool hackInitialized = EFalse;
void NetworkDisplay::UpdateSDL() {
  // try to move window, to fix SDL2 bug on MacOS (Mojave)
  if (!hackInitialized){
    int x, y;
    SDL_GetWindowPosition(screen, &x, &y);
    SDL_SetWindowPosition(screen, x+1, y+1);
    SDL_SetWindowPosition(screen, x, y);
    hackInitialized = ETrue;
  }

  void *screenBuf;
  TInt pitch;

  if (0 == SDL_LockTexture(texture, ENull, &screenBuf, &pitch)) {
    auto *screenBits = (TUint32 *) screenBuf;
    TRGB *palette    = displayBitmap->GetPalette();

    for (TInt16 y = 0; y < SCREEN_HEIGHT; y++) {
      TUint8 *ptr = &displayBitmap->mPixels[y * displayBitmap->GetPitch()];

      for (TInt x = 0; x < SCREEN_WIDTH; x++) {
        TUint8  pixel = *ptr++;
        TUint32 color = palette[pixel].rgb888();
        *screenBits++ = color;
      }
    }

//    (TUint32 *) screenBuf;
//    screenBits = (TUint32 *) screenBuf;
//    Dump(screenBits, renderBitmap->mWidth);
//    Dump(displayBitmap->mPixels, displayBitmap->mWidth, displayBitmap->mHeight);
    SDL_UnlockTexture(texture);
  }
  else {
    printf("Can't lock texture (%s)\n", SDL_GetError());
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, ENull, ENull); // Render texture to entire window
  SDL_RenderPresent(renderer);              // Do update

}


void NetworkDisplay::Update() {
  SwapBuffers();

#ifdef __USE_SDL_VIDEO__
  UpdateSDL();
#endif

  TRGB  *palette = displayBitmap->GetPalette();

  // Networked Matrix Display
  matrixDisplay->LockMutex();
  auto *matrixInputBuff = (TUint16 *)matrixDisplay->GetInputBuffer();

  for (TInt16 y = 0; y < SCREEN_HEIGHT; y++) {
    TUint8    *ptr = &displayBitmap->mPixels[y * displayBitmap->GetPitch()];

    for (TInt x = 0; x < SCREEN_WIDTH; x++) {
      TUint8  pixel = *ptr++;
      TUint16 color = palette[pixel].rgb565();
      *matrixInputBuff++ = color;
    }
  }
  matrixDisplay->UnlockMutex();

  matrixDisplay->Update();

  NextFrameDelay();
}

NetworkDisplay::~NetworkDisplay() {
  // Close and destroy the window
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(screen);

  // Clean up
  SDL_Quit();

}

#endif