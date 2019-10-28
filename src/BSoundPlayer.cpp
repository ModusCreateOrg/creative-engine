#include "BSoundPlayer.h"
#include <pthread.h>

TBool BSoundPlayer::mAudioPaused = true;

#define DISABLE_AUDIO
//#undef DISABLE_AUDIO


#include "libxmp/xmp.h"


#ifdef __XTENSA__

#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include <string.h> // For memset
#include "Memory.h"

#define SAMPLE_RATE (22050)
#define AUDIO_BUFF_SIZE 12
#endif


#ifdef __MODUS_TARGET_SDL2_AUDIO__

#include <SDL.h>
#include <SDL_audio.h>

#define SAMPLE_RATE (44100)

#endif

#ifdef __DINGUX__
#include <SDL/SDL.h>
#define SAMPLE_RATE (44100)
#endif





xmp_context xmpContext1;
xmp_context xmpContext2;
xmp_context *currentContext = &xmpContext1;

volatile bool musicFileLoaded = false;


BSoundPlayer::BSoundPlayer() {
#ifdef DISABLE_AUDIO
  return;
#endif
  xmpContext1 = xmp_create_context();
  xmpContext2 = xmp_create_context();
//  printf("xmpContext1 = %p\n", &xmpContext1);
//  printf("xmpContext2 = %p\n", &xmpContext2);

  mMuted = false;
  BSoundPlayer::mAudioPaused = false;
}

BSoundPlayer::~BSoundPlayer() {
#ifdef DISABLE_AUDIO
  return;
#endif
  Reset();
#ifndef __XTENSA__
  SDL_CloseAudio();
  SDL_Delay(50);
#endif

  // Cleanup LibXMP
  xmp_end_player(xmpContext1);
  xmp_end_player(xmpContext2);
  xmp_end_smix(xmpContext1);
  xmp_end_smix(xmpContext2);
  xmp_release_module(xmpContext1);
  xmp_release_module(xmpContext2);
  xmp_free_context(xmpContext1);
  xmp_free_context(xmpContext2);

  // Release songs memory
  FreeMem(mSongSlots);
}

bool WARNED_OF_PLAY_BUFFER = false;

#ifdef __DINGUX__
static Uint32 audio_len;
static Uint8 *audio_pos;
#endif

static void fillBuffer(void *audioBuffer, size_t length) {
#ifdef DISABLE_AUDIO
  return;
#endif

  if (musicFileLoaded && ! BSoundPlayer::mAudioPaused) {
    int result = xmp_play_buffer(*currentContext, audioBuffer, (int)length, 0);

    if (result != 0) {
      if (!WARNED_OF_PLAY_BUFFER) {
        // Something really bad happened, and audio stopped :(
#ifndef PRODUCTION
        #if (defined(__XTENSA__) && defined(DEBUGME)) || !defined(__XTENSA__)
        printf("xmp_play_buffer not zero (result = %i)!\n", result);fflush(stdout);
#endif
#endif
        WARNED_OF_PLAY_BUFFER = true;
      }
      bzero(audioBuffer, length);

    }
    else {

#ifdef __DINGUX__
    length = 0;

      //      length = (length > audio_len) ? audio_len : length;
//      SDL_MixAudio((Uint8 *)audioBuffer, audio_pos, length, SDL_MIX_MAXVOLUME);
//      audio_pos += length;
//      audio_len -= length;
#endif
    }
  }
  else {
    bzero(audioBuffer, length);
  }

#ifndef __XTESNA__

  if (audio.IsMuted()) {
    printf("audio is muted!\n");
    bzero(audioBuffer, length);
  }
#endif
}


#ifdef __XTENSA__

// ESP32 style timer
static void timerCallback(void *arg) {
#ifdef DISABLE_AUDIO
  return;
#endif
  fillBuffer(audio.mAudioBuffer, AUDIO_BUFF_SIZE);
  // Need to submit to the audio driver.
  audio.Submit(audio.mAudioBuffer, AUDIO_BUFF_SIZE >> 2);
}

#else

// SDL style timer
static void timerCallback(void *udata, Uint8 *audioBuffer, int length) {
  fillBuffer(audioBuffer, length);
}

#endif


void BSoundPlayer::Init(TUint8 aNumberFxChannels, TUint8 aNumberFxSlots) {
#ifdef DISABLE_AUDIO
  return;
#endif
  audio.Init(&timerCallback);

  #ifdef __MODUS_TARGET_SDL2_AUDIO__
  // Kick off SDL audio engine
  SDL_PauseAudio(0);
  #endif

  #ifdef __DINGUX__
  SDL_PauseAudio(0);
  #endif

  mNumberFxChannels = aNumberFxChannels;
  mNumberFxSlots = aNumberFxSlots;
}

TBool BSoundPlayer::LoadSong(BRaw *aSong) {
  xmp_context *loadingContext = (currentContext == &xmpContext1) ? &xmpContext2 : &xmpContext1;
#ifdef DISABLE_AUDIO
  return 0;
#endif

  int loadResult = xmp_load_module_from_memory(*loadingContext, aSong->mData, aSong->mSize);

  if (loadResult < 0) {
    loadResult = xmp_load_module_from_memory(*loadingContext, aSong->mData, aSong->mSize);;
  }

  return loadResult == 0;
}


TBool BSoundPlayer::LoadEffect(TUint16 aResourceId, TUint8 aSlotNumber) {
  xmp_context *loadingContext = (currentContext == &xmpContext1) ? &xmpContext2 : &xmpContext1;

#ifdef DISABLE_AUDIO
  return false;
#endif
  BRaw *effect = LoadEffectResource(aResourceId, aSlotNumber);

  int result = xmp_smix_load_sample_from_memory(*loadingContext, aSlotNumber, effect->mData, effect->mSize);


  if (result < 0) {
    printf("xmp_smix_load_sample_from_memory failed (%i)!\n", result);
  }
  return result == 0;
}




TUint8 sfxChannel = 0;

TBool BSoundPlayer::StopMusic() {

  // Should we test for XMP_STATE_UNLOADED, XMP_STATE_PLAYING?
  xmp_stop_module(*currentContext);
  return true;
}

TBool BSoundPlayer::Reset() {

#ifndef __MODUS_TARGET_SDL2_AUDIO__
  SDL_PauseAudio(1);
#endif

#ifdef __DINGUX__

  SDL_PauseAudio(1);
#endif


  PauseMusic(true);
  MuteMusic(ETrue);
  audio.Mute(true);
  musicFileLoaded = false;


//  xmp_stop_module(*aContext);
//  xmp_end_player(*aContext);
//  xmp_release_module(*aContext);
//  xmp_end_smix(*aContext);

  return true;
}

TBool BSoundPlayer::SetVolume(TFloat aPercent) {

  if (xmpContext1) {
    if (aPercent > 1.0f) {
      aPercent = 1.0f;
    }

    if (aPercent < 0.0f) {
      aPercent = 0;
    }

#ifdef __XTENSA__
    mMusicVolume = (TUint8)(aPercent * 150);
    mEffectsVolume = (TUint8)(aPercent * 140);
#else
    mMusicVolume = (TUint8)(aPercent * 200);
    mEffectsVolume = (TUint8)(aPercent * 254);
#endif
    xmp_set_player(*currentContext, XMP_PLAYER_VOLUME, mMusicVolume);
    return true;
  }

  return false;
}

TBool BSoundPlayer::SetMusicVolume(TFloat aPercent) {
#ifdef DISABLE_AUDIO
  return false;
#endif
  if (aPercent > 1.0f) {
    aPercent = 1.0f;
  }
  if (aPercent < 0.0f) {
    aPercent = 0;
  }


#ifdef __XTENSA__
  mMusicVolume = (TUint8)(aPercent * 120);
#else
  mMusicVolume = (TUint8)(aPercent * 200);
#endif

  xmp_set_player(*currentContext, XMP_PLAYER_VOLUME, mMusicVolume);
  return true;
}

TBool BSoundPlayer::SetEffectsVolume(TFloat aPercent) {
#ifdef DISABLE_AUDIO
  return false;
#endif
  if (xmpContext1) {
    if (aPercent > 1.0f) {
      aPercent = 1.0f;
    }

    if (aPercent < 0.0f) {
      aPercent = 0;
    }

#ifdef __XTENSA__
    mEffectsVolume = (TUint8)(aPercent * 120);
#else
    mEffectsVolume = (TUint8)(aPercent * 200);
#endif
    xmp_set_player(*currentContext, XMP_PLAYER_SMIX_VOLUME, mEffectsVolume);
    return true;
  }

  return false;
}



TBool BSoundPlayer::PlaySfx(TInt aSoundNumber) {
//  printf("%s(%i)\n", __FUNCTION__, aSoundNumber);

#ifdef DISABLE_AUDIO
  return false;
#endif

#ifndef PRODUCTION
#if (defined(__XTENSA__) && defined(DEBUGME)) || !defined(__XTENSA__)
  printf("SFX: %i\n", aSoundNumber); fflush(stdout);
#endif
#endif

  if (! musicFileLoaded) {
#ifndef PRODUCTION
#if (defined(__XTENSA__) && defined(DEBUGME)) || !defined(__XTENSA__)
    printf("%s: No Music file loaded! Cannot play sound effects!\n", __FUNCTION__);
#endif
#endif
    return false;
  }

  xmp_smix_play_sample(*currentContext, aSoundNumber, 60, mEffectsVolume, sfxChannel);

  sfxChannel++;
  if (sfxChannel >= 2) {
    sfxChannel = 0;
  }
  return true;
}

TBool BSoundPlayer::PlayMusic(TInt16 aResourceId) {

#ifdef DISABLE_AUDIO
  return false;
#endif

  if (aResourceId == mCurrentSongLoaded) {
    return false;
  }
  xmp_context *loadingContext = (currentContext == &xmpContext1) ? &xmpContext2 : &xmpContext1;

  Reset();

  xmp_stop_module(*loadingContext);
  xmp_end_smix(*loadingContext);
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

  TBool loadResult = LoadSongSlot(aResourceId);

  if (loadResult) {
    musicFileLoaded = true;
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

  xmp_start_smix(*loadingContext, mNumberFxChannels, mNumberFxSlots);
  LoadEffects();

  xmp_start_player(*loadingContext, SAMPLE_RATE, 0);
  xmp_set_player(*loadingContext, XMP_PLAYER_VOLUME, mMusicVolume);
  xmp_set_player(*loadingContext, XMP_PLAYER_SMIX_VOLUME, mEffectsVolume); //Volume is governed globally via mEffectsVolume
  xmp_set_player(*loadingContext, XMP_PLAYER_MIX, 0);


  MuteMusic(EFalse);
  PauseMusic(false);
  audio.Mute(false);

#ifndef __XTENSA__
  SDL_PauseAudio(0);
#endif

  currentContext = loadingContext;

  return ETrue;
}


