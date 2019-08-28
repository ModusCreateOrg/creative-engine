#ifdef __DINGUX__

#ifndef LDKAUDIO_H
#define LDKAUDIO_H



#include "AudioBase.h"
#include <SDL.h>


#define BUFFER_FRAMES 3
// 48000 Hz maximum; 1/50 of a second; 3 frames to hold (2 plus a bit extra)
#define BUFFER_SAMPLES (48000 / 50 * (BUFFER_FRAMES + 1))

#define SAMPLE_RATE (44100)

typedef void (*TAudioDriverCallback)(void *userdata, Uint8 *stream, int len);



class SDL2Audio : public AudioBase {
public:

  void Init(TAudioDriverCallback aCallback) {
    SDL_AudioSpec audioSpec;

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
      return;
    }

    audioSpec.freq = SAMPLE_RATE;
    audioSpec.format = AUDIO_S16;
    audioSpec.channels = 2;
    audioSpec.samples = 100;
    audioSpec.callback = aCallback;

    if (SDL_OpenAudio(&audioSpec, nullptr) < 0) {
      fprintf(stderr, "%s\n", SDL_GetError());
    }

  }

  void SetVolume(TFloat value) override {}

  void Terminate() override {
//    SDL_CloseAudio();
  }

  ~SDL2Audio() {
//    Terminate();
  }
};




#endif //MODITE_LDKAUDIO_H
#endif
