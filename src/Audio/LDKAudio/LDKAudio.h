#ifdef __DINGUX__

#ifndef LDKAUDIO_H
#define LDKAUDIO_H



#include "AudioBase.h"
#include <SDL.h>


#define BUFFER_FRAMES 4
// 48000 Hz maximum; 1/50 of a second; 3 frames to hold (2 plus a bit extra)
#define BUFFER_SAMPLES (48000 / 50 * (BUFFER_FRAMES + 1))

#define SAMPLE_RATE (44100)

typedef void (*TAudioDriverCallback)(void *userdata, Uint8 *stream, int len);


class LDKAudio : public AudioBase {
public:

  void Init(TAudioDriverCallback aCallback) {
    SDL_AudioSpec audioSpec;

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
      printf("Could not init audio!!\n");
      return;
    }

    audioSpec.freq = SAMPLE_RATE;
    audioSpec.format = AUDIO_S16;
    audioSpec.channels = 2;
    audioSpec.samples = 300;
    audioSpec.callback = aCallback;

    if (SDL_OpenAudio(&audioSpec, nullptr) < 0) {
      fprintf(stderr, "%s\n", SDL_GetError());
    }

    printf("AUdio init ok\n");
  }

  void SetVolume(TFloat value) override {}

  void Terminate() override {
    SDL_CloseAudio();
  }

  ~LDKAudio() {
//    Terminate();
  }
};




#endif //MODITE_LDKAUDIO_H
#endif
