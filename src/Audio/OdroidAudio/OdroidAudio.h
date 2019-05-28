#ifdef __XTENSA__

#ifndef GENUS_ODROIDAUDIO_H
#define GENUS_ODROIDAUDIO_H

#define SAMPLE_RATE (22050)


#include "AudioBase.h"
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "driver/i2s.h"
#include "driver/rtc_io.h"

typedef void (*TAudioDriverCallback)(void *arg);

class OdroidAudio : public AudioBase {
public:
  OdroidAudio() = default;
  ~OdroidAudio() = default;

  void Init(TAudioDriverCallback aDriverCallback);

  void Terminate() override;

  void SetVolume(TFloat value) override {
    TFloat newValue = mAudioVolume + value;

    if (newValue > .124f) {
      mAudioVolume = 0;
    }
    else {
      mAudioVolume = newValue;
    }

#ifndef PRODUCTION
    #ifdef DEBUGME
  printf("mAudioVolume = %f\n", mAudioVolume);
#endif
#endif
  }


  void Submit(TInt16 *stereoAudioBuffer, TInt frameCount);
  static void i2sTimerCallback(void *arg);
};


#endif //GENUS_ODROIDAUDIO_H

#endif