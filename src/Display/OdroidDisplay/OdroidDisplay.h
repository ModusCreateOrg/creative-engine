#ifdef __XTENSA__

#ifndef ODROIDDISPLAY_H
#define ODROIDDISPLAY_H


#include "DisplayDefines.h"
#include "DisplayBase.h"

#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_event.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "driver/rtc_io.h"

#include <string.h>

class OdroidDisplay : public DisplayBase {

public:
  OdroidDisplay() = default;

  void Init() override;

  void Update() override;

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

  TUint16 color565(TUint8 b, TUint8 r, TUint8 g) override {
    // lifted from Display2.cpp
    TUint16 blue = (b & 0b11111000) << 5;
    TUint16 red = (r & 0b11111000);

    TUint16 g2 = (g & 0b00011100) << 11;
    TUint16 g1 = (g & 0b11100000) >> 5;
    TUint16 green = g1 + g2;


    // For the ILI9341 display , data is organized like this:
    //     000   00000   00000  000
    //    G456  B12345  R12345 G123
    // Green high bits (4 - 6), Blue bits, Red bits, Green low bits (1 - 3)
    TUint16 final = (TUint16)(red + green + blue);
    return final;
  }

  void SetBrightness(int value);


};



#endif //ODROIDDISPLAY_H


#endif