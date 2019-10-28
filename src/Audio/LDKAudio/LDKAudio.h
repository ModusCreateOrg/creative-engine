#ifdef __DINGUX__

#ifndef LDKAUDIO_H
#define LDKAUDIO_H



#include "AudioBase.h"
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_audio.h>


#define BUFFER_FRAMES 4
// 48000 Hz maximum; 1/50 of a second; 3 frames to hold (2 plus a bit extra)
#define BUFFER_SAMPLES (48000 / 50 * (BUFFER_FRAMES + 1))

#define SAMPLE_RATE (44100)

typedef void (*TAudioDriverCallback)(void *userdata, Uint8 *stream, int len);


class LDKAudio : public AudioBase {
public:

  void Init(TAudioDriverCallback aCallback) {
    SDL_AudioSpec audioSpec;

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
      fprintf(stderr, "Could Initiate SDL_InitSubSystem()! %s\n", SDL_GetError());
      fprintf(stderr, "EXITING...");
      exit(-1);
    }


    char driverName[8];
    SDL_AudioDriverName(driverName, 8);

    fprintf(stderr, "Loading SDL sound with %s driver...\n", driverName);

    audioSpec.freq = SAMPLE_RATE;
    audioSpec.format = AUDIO_S16;
    audioSpec.channels = 2;
    audioSpec.samples = 512;
    audioSpec.callback = aCallback;

    SDL_AudioSpec obtained;

//    if (SDL_OpenAudio(&audioSpec, nullptr) < 0) {
    if (SDL_OpenAudio(&audioSpec, &obtained) < 0) {
      fprintf(stderr, "Could Initiate SDL_OpenAudio()! %s\n", SDL_GetError());
      fprintf(stderr, "EXITING...");
      exit(-1);
    }
    printf("Device opened with %d Hz, %d channels and sample buffer w/ %d samples.\n",
           obtained.freq, obtained.channels, obtained.samples);
    SDL_PauseAudio(0);

//
//    Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048);

//    Mix_AllocateChannels(0);

    printf("Audio init ok\n");
  }

  void SetVolume(TFloat value) override {

  }

  void Terminate() override {
    SDL_CloseAudio();
  }

  ~LDKAudio() {
    Terminate();
  }
};




#endif //MODITE_LDKAUDIO_H
#endif
