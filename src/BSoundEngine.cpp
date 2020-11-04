#include "BSoundEngine.h"
#include <pthread.h>


//#define DISABLE_AUDIO
//#undef DISABLE_AUDIO


#include "libxmp/xmp.h"


xmp_context xmpContext1;
xmp_context xmpContext2;
xmp_context *currentContext = &xmpContext1;

volatile bool musicFileLoaded = false;



BSoundEngine::BSoundEngine() {
//  mAudioVolume = .5;
  mAudioBuffer = ENull;
  mMuted = ETrue;
  musicFileLoaded = EFalse;
//  mMusicVolume = 200;

  BSoundEngine::mAudioPaused = false;
#ifdef DISABLE_AUDIO
  return;
#endif
  xmpContext1 = xmp_create_context();
  xmpContext2 = xmp_create_context();
}

BSoundEngine::~BSoundEngine() {
#ifdef DISABLE_AUDIO
  return;
#endif

  Reset();
  Mixer_CloseAudio();
  SDL_Delay(50);

  for (TUint8 i = 0; i < mNumEffects; i++) {
    Mixer_FreeChunk(mEffects[i]);
  }
  delete[] mEffects;

  SDL_CloseAudio();

  // Cleanup LibXMP
  xmp_end_player(xmpContext1);
  xmp_end_player(xmpContext2);
//  xmp_end_smix(xmpContext1);
//  xmp_end_smix(xmpContext2);
  xmp_release_module(xmpContext1);
  xmp_release_module(xmpContext2);
  xmp_free_context(xmpContext1);
  xmp_free_context(xmpContext2);
}

bool WARNED_OF_PLAY_BUFFER = false;

static void fillBuffer(Uint8 *audioBuffer, size_t length) {
//  printf("Mixer_PlayingMusic() = %i\n",Mixer_PlayingMusic());

#ifdef DISABLE_AUDIO
  return;
#endif

  if (musicFileLoaded && ! soundEngine.mAudioPaused) {
//    int result = 0;
    int result = xmp_play_buffer(*currentContext, audioBuffer, (int)length, 0);
//    printf("xmp_play_buffer result = %i , length = %i\n",  result, (int)length);
    if (result != 0) {
      if (!WARNED_OF_PLAY_BUFFER) {
        // Something really bad happened, and audio stopped :(
#ifndef PRODUCTION
        printf("xmp_play_buffer not zero (result = %i)!\n", result);fflush(stdout);
#endif
      }
      bzero(audioBuffer, length);
    }
  }
  else {
    bzero(audioBuffer, length);
  }


  if (soundEngine.IsMuted()) {
    bzero(audioBuffer, length);
  }

  Mixer_MixChannels(audioBuffer, length);
}

// SDL style timer
static void timerCallback(void *udata, Uint8 *audioBuffer, int length) {
  fillBuffer(audioBuffer, length);
}

void BSoundEngine::InitAudioEngine(TUint8 aNumSfxChannels, TUint8 aNumSfxFiles) {
#ifdef DISABLE_AUDIO
  return;
#endif
  SDL_AudioSpec audioSpec;

  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    return;
  }

  audioSpec.freq = SAMPLE_RATE;
  audioSpec.format = AUDIO_S16;
  audioSpec.channels = 2;
  audioSpec.size = 500;
  audioSpec.callback = timerCallback;

#ifdef __DINGUX__
  audioSpec.samples = 512;
#else
  audioSpec.samples = 1024;
#endif

//  audioSpec.callback = timerCallback;

//  if (SDL_OpenAudio(&audioSpec, nullptr) < 0) {
//    fprintf(stderr, "%s\n", SDL_GetError());
//  }
  mNumEffects = aNumSfxFiles;
  mNumSfxChannels = aNumSfxChannels;
  mEffects = new Mixer_Chunk*[aNumSfxFiles];

//  int result = Mixer_OpenAudio(&audioSpec, );
  int result = SDL_OpenAudio(&audioSpec, nullptr);
  if (result < 0) {
    fprintf(stderr, "%s\n", SDL_GetError());
    fflush(stdout);
    SDL_Delay(500);
    exit(-1);
    return;
  }
  Mixer_SetAudioSpec(&audioSpec);
  Mixer_AllocateChannels(mNumSfxChannels);

  // Kick off SDL audio engine
  SDL_PauseAudio(0);

  mNumberFxChannels = aNumSfxChannels;
}

TBool BSoundEngine::LoadSong(BRaw *aSong) {
#ifdef DISABLE_AUDIO
  return 0;
#endif
  xmp_context *loadingContext = (currentContext == &xmpContext1) ? &xmpContext2 : &xmpContext1;

  int loadResult = xmp_load_module_from_memory(*loadingContext, aSong->mData, aSong->mSize);

  if (loadResult < 0) {
    loadResult = xmp_load_module_from_memory(*loadingContext, aSong->mData, aSong->mSize);;
  }

  return loadResult == 0;
}

TBool BSoundEngine::LoadEffect(TUint8 aSfxIndex, TUint16 aResourceId, TUint8 aResourceSlot) {
#ifdef DISABLE_AUDIO
  return 0;
#endif
  if (gResourceManager.GetRaw(aResourceSlot)) {
    gResourceManager.ReleaseRawSlot(aResourceId);
  }

  gResourceManager.LoadRaw(aResourceId, aResourceSlot);

  BRaw *rawEffect = gResourceManager.GetRaw(aResourceSlot);

  SDL_RWops *file = SDL_RWFromMem(rawEffect->mData, rawEffect->mSize);
  Mixer_Chunk *chunk = Mixer_LoadWAV_RW(file, 1);

  mEffects[aSfxIndex] = chunk;
  return ETrue;
}


TBool BSoundEngine::PlaySfx(TInt aSoundNumber, TInt8 aChannel) {
#ifdef DISABLE_AUDIO
  return 0;
#endif
  Mixer_Chunk  *chunk = mEffects[aSoundNumber];

  int result = Mixer_PlayChannel(aChannel, chunk, 0);
  if (result < 0) {
    printf("ERROR: %s\n", SDL_GetError());
  }

  return ETrue;
}



TBool BSoundEngine::StopMusic() {
  xmp_stop_module(*currentContext);
  return true;
}


TBool BSoundEngine::PlayMusic(BRaw *aRawFile, TInt16 aResourceId) {

#ifdef DISABLE_AUDIO
  return false;
#endif

  if (aResourceId == mCurrentSongLoaded) {
    return false;
  }
  xmp_context *loadingContext = (currentContext == &xmpContext1) ? &xmpContext2 : &xmpContext1;

  Reset();

  xmp_stop_module(*loadingContext);
  xmp_end_player(*loadingContext);

  if (loadingContext == &xmpContext1) {
    xmp_free_context(xmpContext1);
    xmpContext1 = xmp_create_context();
    loadingContext = &xmpContext1;
  }
  else {
    xmp_free_context(xmpContext2);
    xmpContext2 = xmp_create_context();
    loadingContext = &xmpContext2;
  }

  TBool loadResult = LoadSong(aRawFile);

  if (loadResult) {
    musicFileLoaded = ETrue;
    mCurrentSongLoaded = aResourceId;
  }

  if (!musicFileLoaded) {
#ifndef PRODUCTION
    #if (defined(__XTENSA__) && defined(DEBUGME)) || !defined(__XTENSA__)
    printf("MUSIC LOADING FAILED!\n"); fflush(stdout);
#endif
#endif
    return EFalse;
  }

  xmp_start_player(*loadingContext, SAMPLE_RATE, 0);
  printf("Setting loading context volume %i\n", mMusicVolume);
  xmp_set_player(*loadingContext, XMP_PLAYER_VOLUME, mMusicVolume);
  xmp_set_player(*loadingContext, XMP_PLAYER_MIX, 0);

  MuteMusic(EFalse);
  PauseMusic(false);

#ifndef __XTENSA__
  SDL_PauseAudio(0);
#endif

  currentContext = loadingContext;

  return ETrue;
}


TBool BSoundEngine::Reset() {
  SDL_PauseAudio(1);
  PauseMusic(ETrue);
  MuteMusic(ETrue);
  musicFileLoaded = EFalse;
  return true;
}

TBool BSoundEngine::SetVolume(TFloat aPercent) {

  if (xmpContext1) {
    if (aPercent > 1.0f) {
      aPercent = 1.0f;
    }

    if (aPercent < 0.0f) {
      aPercent = 0;
    }

    printf("setVolume %2f\n", aPercent);

    mMusicVolume = (TUint8)(aPercent * 200);
    mEffectsVolume = (TUint8)(aPercent * MIXER_MAX_VOLUME);
    xmp_set_player(*currentContext, XMP_PLAYER_VOLUME, mMusicVolume);
    return ETrue;
  }

  return EFalse;
}

TBool BSoundEngine::SetMusicVolume(TFloat aPercent) {
#ifdef DISABLE_AUDIO
  return false;
#endif
  if (aPercent > 1.0f) {
    aPercent = 1.0f;
  }
  if (aPercent < 0.0f) {
    aPercent = 0;
  }
  mMusicVolume = (TUint8)(aPercent * 200);
  xmp_set_player(*currentContext, XMP_PLAYER_VOLUME, mMusicVolume);
  return ETrue;
}

TBool BSoundEngine::SetEffectsVolume(TFloat aPercent) {
#ifdef DISABLE_AUDIO
  return 0;
#endif
  if (aPercent > 1.0f) {
    aPercent = 1.0f;
  }

  if (aPercent < 0.0f) {
    aPercent = 0;
  }

  auto volume = (TUint8)(aPercent * MIXER_MAX_VOLUME);
  printf("SetEffectsVolume volume = %i, pct = %2f\n", volume, aPercent);
  for (int i = 0; i < mNumSfxChannels; i++) {
    Mixer_Volume(i, volume);
  }
  return ETrue;
}

BSoundEngine soundEngine;