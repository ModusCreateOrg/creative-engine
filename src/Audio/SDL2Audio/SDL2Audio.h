#ifdef __MODUS_TARGET_SDL2_AUDIO__

#ifndef GENUS_SDL2AUDIO_H
#define GENUS_SDL2AUDIO_H

#include "AudioBase.h"
#include <SDL.h>
#include <SDL_audio.h>

#define SAMPLE_RATE (44100)

typedef SDL_AudioCallback TAudioDriverCallback;


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
    audioSpec.samples = 1024;
    audioSpec.size = 500;
    audioSpec.callback = aCallback;

    if (SDL_OpenAudio(&audioSpec, nullptr) < 0) {
      fprintf(stderr, "%s\n", SDL_GetError());
    }

  }

  void SetVolume(TFloat value) override {}

  void Terminate() override {
    SDL_CloseAudio();
  }

  ~SDL2Audio() {
    Terminate();
  }
};


#endif //GENUS_SDL2AUDIO_H


#endif