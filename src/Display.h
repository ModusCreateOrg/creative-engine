/**
 * The purpose of this class is to isolate or encapsulate hardware display differences between targets.
 *
 * For example, when running/debugging on host, accessing display might be through libsdl2,
 * while on target, direct hardware or I/O might be required to update the screen.
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include "BBitmap.h"

#ifdef __XTENSA__
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_event.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "driver/rtc_io.h"

#include <string.h>
#endif

// screen attributes
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_DEPTH 8

#define DISPLAY_WIDTH (SCREEN_WIDTH)
#define DISPLAY_HEIGHT (SCREEN_HEIGHT)

class Display {
public:
  Display();

  virtual ~Display();

public:
  void Init();
  void SetPalette(TRGB aPalette[], TInt aIndex=0, TInt aCount = 256) {
    displayBitmap->SetPalette(aPalette, aIndex, aCount);
    renderBitmap->SetPalette(aPalette, aIndex, aCount);
  }
  void SetPalette(BBitmap *aBitmap, TInt aIndex=0, TInt aCount = 256) {
    displayBitmap->SetPalette(aBitmap, aIndex, aCount);
    renderBitmap->SetPalette(aBitmap, aIndex, aCount);
  }

  void SetColor(TUint8 aIndex, TUint8 aRed, TUint8 aGreen, TUint8 aBlue) {
    displayBitmap->SetColor(aIndex, aRed, aGreen, aBlue);
    renderBitmap->SetColor(aIndex, aRed, aGreen, aBlue);
  }
  void Update();




#ifdef __XTENSA__

  static void DisplayTask(void* arg);
  static void SpiTask(void* arg);
  static void LockDisplay();
  static void UnlockDisplay();
  static void WriteFrame(TUint8* frameBuffer, TUint16* palette);
  static spi_transaction_t* GetSpiTransaction();
  static void PutSpiTransaction(spi_transaction_t *t);
  static void InitializeSPI();

  static TUint16* GetLineBufferQueue();
  static void PutLineBufferQueue(TUint16 *buffer);
  static void SendDisplayCommand(const TUint8 cmd);
  static void SendDisplayData(const TUint8 *data, int len);
  static void SpiPreTransferCallback(spi_transaction_t *t);
  static void SendDisplayBootProgram();
  static void SendResetDrawing(TUint8 left, TUint8 top, TUint16 width, TUint8 height);
  static void SendContinueWait();
  static void SendContinueLine(TUint16 *line, TInt width, TInt lineCount);
  static void InitializeBacklight();
  static void Clear(TUint16 color); // Doesn't seem to be used.

  TUint16 color565(TUint8 b, TUint8 r, TUint8 g) {
    // lifted from Display2.cpp
    uint16_t blue = (b & 0b11111000) << 5;
    uint16_t red = (r & 0b11111000);

    uint16_t g2 = (g & 0b00011100) << 11;
    uint16_t g1 = (g & 0b11100000) >> 5;
    uint16_t green = g1 + g2;


    // For the ILI9341 display , data is organized like this:
    //     000   00000   00000  000
    //    G567  B12345  R12345 G123
    // Green high bits (5 - 7), Blue bits, Red bits, Green low bits (1 - 3)
    uint16_t final = (uint16_t)(red + green + blue);
    return final;
  }



#else
  TUint16 color565(TUint8 red, TUint8 green, TUint8 blue) {
    // lifted from Display2.cpp
    // maybe we want to return something else for SDL
    return ((blue & 0xF8) << 8) | ((red & 0xFC) << 3) | ((green & 0xF8) >> 3);
  }
#endif

protected:
  // 2 bitmaps for double buffering
  BBitmap *mBitmap1, *mBitmap2;
public:
  BBitmap *displayBitmap, *renderBitmap;
};

extern Display gDisplay;
extern TRect gScreenRect;

#endif //DISPLAY_H
