#ifndef AUDIO_H
#define AUDIO_H

#include "BTypes.h"
#include "BBase.h"

extern short *audioBuffer;


#ifdef __XTENSA__
/***** ODROID GO *****/
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "driver/i2s.h"
#include "driver/rtc_io.h"


typedef void (*TAudioDriverCallback)(void *arg);
#else
/***** Mac/Linux *****/
#include <SDL2/SDL_audio.h>

//typedef void (*)(void *,Uint8 *,int) SDL_AudioCallback
typedef SDL_AudioCallback TAudioDriverCallback;

#endif



class Audio : public BBase {
protected:
  TBool mMuted;

public:
  short *mAudioBuffer;

public:
  Audio();
  virtual ~Audio();
  void Init(TAudioDriverCallback aDriverCallback);

  void SetVolume(TFloat value);
  //  void ChangeVolume();
  TFloat GetVolume();
  void Terminate();
  void Mute(TBool aMuted) {
    mMuted = aMuted;
  }
  TBool IsMuted() {
    return mMuted;
  }

#ifdef __XTENSA__
  void Submit(TInt16 *stereoAudioBuffer, TInt frameCount);
  static void i2sTimerCallback(void *arg);
#endif

};


extern Audio audio;

#endif
