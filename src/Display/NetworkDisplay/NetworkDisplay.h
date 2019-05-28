#ifdef __MODUS_TARGET_NETWORK_DISPLAY__

#ifndef GENUS_MATRIX_DISPLAY_NETWORKDISPLAY_H
#define GENUS_MATRIX_DISPLAY_NETWORKDISPLAY_H


#include "BBase.h"
#include "BBitmap.h"


#include "DisplayBase.h"

// screen attributes
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_DEPTH 8

#define DISPLAY_WIDTH (SCREEN_WIDTH)
#define DISPLAY_HEIGHT (SCREEN_HEIGHT)

class NetworkDisplay : public DisplayBase {
public:
  NetworkDisplay();
  ~NetworkDisplay();

  void Update() override;
  void UpdateSDL();

  // For compatibility
  void Init() override;

  TUint16 color565(TUint8 red, TUint8 green, TUint8 blue) override{
    // lifted from Display2.cpp
    // maybe we want to return something else for SDL
    return ((blue & 0xF8) << 8) | ((red & 0xFC) << 3) | ((green & 0xF8) >> 3);
  }
};

//extern NetworkDisplay gNetworkDisplay;



#endif //GENUS_MATRIX_DISPLAY_NETWORKDISPLAY_H

#endif //#ifdef __MODUS_TARGET_NETWORK_DISPLAY__