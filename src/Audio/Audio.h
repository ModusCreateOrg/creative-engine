//#ifndef AUDIO_H
//#define AUDIO_H
//
//
////#ifdef __DINGUX__
////#include "LDKAudio.h"
////class Audio : public LDKAudio {};
////#endif
////
////// Desktop / Linux / Raspberry Pi
////#include "SDL2Audio.h"
////class Audio : public SDL2Audio {};
////#endif
//
//
//
//class Audio : public BBase {
//protected:
//    TBool mMuted;
//    TFloat mAudioVolume;
//
//public:
//  Audio() {
//    mAudioVolume = .5;
//    mMuted = ETrue;
//    mAudioBuffer = ENull;
//  }
//
//  Mix_Chunk **mEffects;
//  TUint8  mNumEffects;
//
//  void Init(TAudioDriverCallback aCallback, TUint8 aNumSfxFiles, TInt8 aNumMixChannels) {
//    SDL_AudioSpec audioSpec;
//
//    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
//      return;
//    }
//
//    audioSpec.freq = SAMPLE_RATE;
//    audioSpec.format = AUDIO_S16;
//    audioSpec.channels = 2;
//    audioSpec.samples = 1024;
//    audioSpec.size = 500;
//    audioSpec.callback = aCallback;
//
////    if (SDL_OpenAudio(&audioSpec, nullptr) < 0) {
////      fprintf(stderr, "%s\n", SDL_GetError());
////    }
//    mNumEffects = aNumSfxFiles;
//    mEffects = new Mix_Chunk*[aNumSfxFiles];
//
//    int result = Mix_OpenAudio(audioSpec.freq, audioSpec.format, audioSpec.channels, audioSpec.samples);
//    if (result < 0) {
//      fprintf(stderr, "%s\n", Mix_GetError());
//      return;
//    }
//
//    Mix_AllocateChannels(aNumMixChannels);
//    Mix_HookMusic(aCallback, ENull);
//    Mix_VolumeMusic(MIX_MAX_VOLUME);
//    Mix_ResumeMusic();
//
//  }
//
//  ~Audio() {
//    for (TUint8 i = 0; i < mNumEffects; i++) {
//      Mix_FreeChunk(mEffects[i]);
//    }
//    delete[] mEffects;
//
//    Mix_HookMusic(ENull, ENull);
//    SDL_CloseAudio();
//  }
//
//
//
////    virtual void SetVolume(TFloat value) = 0;
//
//  TFloat GetVolume() {
//    return mAudioVolume;
//  }
//
//  void Mute(TBool aMuted) {
//    mMuted = aMuted;
//  }
//
//  TBool IsMuted() {
//    return mMuted;
//  }
//
//  TBool LoadSfxFile(BRaw *aSfxFile, TUint8 aSlotNumber) {
//    SDL_RWops *file = SDL_RWFromMem(aSfxFile->mData, aSfxFile->mSize);
//    mEffects[aSlotNumber] = Mix_QuickLoad_WAV(aSfxFile->mData);
//    printf("mEffects[%i] = %p\n", aSlotNumber, mEffects[aSlotNumber]);
//    return ETrue;
//  }
//
//  TBool PlaySfx(TInt aSoundNumber) {
//    Mix_PlayChannel(1, mEffects[aSoundNumber], 0);
//    return ETrue;
//  }
//
//  short *mAudioBuffer;
//
//
//
//};
//
//
//
//
//
//extern Audio audio;
//
//
//#endif
