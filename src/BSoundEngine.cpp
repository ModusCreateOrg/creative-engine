#include "BSoundEngine.h"
#include <pthread.h>


#define DISABLE_AUDIO
#undef DISABLE_AUDIO


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

  BSoundEngine::mAudioPaused = false;
#ifdef DISABLE_AUDIO
  return;
#endif
  xmpContext1 = xmp_create_context();
  xmpContext2 = xmp_create_context();
//  printf("xmpContext1 = %p\n", &xmpContext1);
//  printf("xmpContext2 = %p\n", &xmpContext2);

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

  Mixer_HookMusic(ENull, ENull);
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

//#ifdef __DINGUX__
//static Uint32 audio_len;
//static Uint8 *audio_pos;
//#endif

static void fillBuffer(void *audioBuffer, size_t length) {
//  printf("Mixer_PlayingMusic() = %i\n",Mixer_PlayingMusic());

#ifdef DISABLE_AUDIO
  return;
#endif

  if (musicFileLoaded && ! soundEngine.mAudioPaused) {
//    int result = 0;
    int result = xmp_play_buffer(*currentContext, audioBuffer, (int)length, 0);
//    printf("xmp_play_buffer result = %i , length = %i\n",  result, length);
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
  audioSpec.samples = 1024;
  audioSpec.size = 500;
//  audioSpec.callback = timerCallback;

//    if (SDL_OpenAudio(&audioSpec, nullptr) < 0) {
//      fprintf(stderr, "%s\n", SDL_GetError());
//    }
  mNumEffects = aNumSfxFiles;
  mNumSfxChannels = aNumSfxChannels;
  mEffects = new Mixer_Chunk*[aNumSfxFiles];

  int result = Mixer_OpenAudio(
    audioSpec.freq,
    audioSpec.format,
    audioSpec.channels,
    audioSpec.samples
  );

  if (result < 0) {
    fprintf(stderr, "%s\n", Mixer_GetError());
    return;
  }
  printf("Allocating mNumSfxChannels = %i \n", mNumSfxChannels);
  Mixer_AllocateChannels(mNumSfxChannels);
  fprintf(stderr, "%s\n", Mixer_GetError());
  Mixer_HookMusic(timerCallback, ENull);

  // Kick off SDL audio engine
  SDL_PauseAudio(0);

  mNumberFxChannels = aNumSfxChannels;
}

TBool BSoundEngine::LoadSong(BRaw *aSong) {
  xmp_context *loadingContext = (currentContext == &xmpContext1) ? &xmpContext2 : &xmpContext1;
//#ifdef DISABLE_AUDIO
//  return 0;
//#endif

  int loadResult = xmp_load_module_from_memory(*loadingContext, aSong->mData, aSong->mSize);

  if (loadResult < 0) {
    loadResult = xmp_load_module_from_memory(*loadingContext, aSong->mData, aSong->mSize);;
  }

  return loadResult == 0;
}

TBool BSoundEngine::LoadEffect(TUint8 aSfxIndex, TUint16 aResourceId, TUint8 aResourceSlot) {
  printf("\nBSoundEngine::LoadEffect(aSfxIndex = %i, aResourceId = %i, aResourceSlot = %i)\n", aSfxIndex, aResourceId, aResourceSlot);

  if (gResourceManager.GetRaw(aResourceSlot)) {
    gResourceManager.ReleaseRawSlot(aResourceId);
  }

  gResourceManager.LoadRaw(aResourceId, aResourceSlot);

  BRaw *rawEffect = gResourceManager.GetRaw(aResourceSlot);
//  printf("rawEffect->mSize %i\n", rawEffect->mSize);

  SDL_RWops *file = SDL_RWFromMem(rawEffect->mData, rawEffect->mSize);
  Mixer_Chunk *chunk = Mixer_LoadWAV_RW(file, 1);
//  mEffects[aSfxIndex] = Mixer_QuickLoad_WAV(rawEffect->mData);
//  Mixer_Chunk *chunk = Mixer_QuickLoad_WAV(rawEffect->mData);
//  printf("Chunk %i : alen %i,  allocated %i, volume %i\n", aSfxIndex, chunk->alen, chunk->allocated, chunk->volume);

  mEffects[aSfxIndex] = chunk;
  gResourceManager.ReleaseRawSlot(aResourceId);
//  printf("Result = %i\n", result);

//  printf("mEffects[%i] = %p\n", aResourceSlot, mEffects[aSfxIndex]);
  return ETrue;
}


TBool BSoundEngine::PlaySfx(TInt aSoundNumber) {
//  return ETrue;
  for (int i = 0; i< mNumberFxChannels; i++) {
    Mixer_Volume(i, MIXER_MAX_VOLUME);
  }
  Mixer_ClearError();
  printf("%s(%i)\n", __FUNCTION__, aSoundNumber);
  Mixer_Chunk  *chunk = mEffects[aSoundNumber];
  printf("Chunk %i : alen %i,  allocated %i, volume %i\n", aSoundNumber, chunk->alen, chunk->allocated, chunk->volume);

  int result = Mixer_PlayChannel(-1, chunk, 0);
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
    mEffectsVolume = (TUint8)(aPercent * 254);
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

//  aPercent = .1;
  mMusicVolume = (TUint8)(aPercent * 200);
//  Mixer_Volume((TUint8)aPercent * MIXER_MAX_VOLUME);
  xmp_set_player(*currentContext, XMP_PLAYER_VOLUME, mMusicVolume);
  return true;
}

TBool BSoundEngine::SetEffectsVolume(TFloat aPercent) {
  TUint8 volume = (TUint8)aPercent * MIXER_MAX_VOLUME;

  for (int i = 0; i < mNumSfxChannels; i++) {
    Mixer_Volume(i, volume);
  }
#ifdef DISABLE_AUDIO
  return false;
#endif
  return false;
}

BSoundEngine soundEngine;