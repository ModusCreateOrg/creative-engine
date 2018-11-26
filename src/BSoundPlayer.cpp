//
// Created by Jesus Garcia on 11/8/18.
//

#include "BSoundPlayer.h"
TBool BSoundPlayer::mAudioPaused = true;

//
// Created by Jesus Garcia on 10/2/18.
//

#include "BSoundPlayer.h"

#define DISABLE_AUDIO
#undef DISABLE_AUDIO



#include "libxmp/xmp.h"


#ifdef __XTENSA__

#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include <string.h> // For memset

#else

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#endif



#ifdef __XTENSA__

#define SAMPLE_RATE (22050)
#define AUDIO_BUFF_SIZE 12
#else

#define SAMPLE_RATE (44100)

#endif



xmp_context xmpContext;

bool musicFileLoaded = false;

BSoundPlayer::BSoundPlayer() {
#ifdef DISABLE_AUDIO
  return;
#endif
  xmpContext = xmp_create_context();


//  mMusicVolume = 46;
//  mEffectsVolume = 96;
  mMuted = false;
  BSoundPlayer::mAudioPaused = false;
}

BSoundPlayer::~BSoundPlayer() {
#ifdef DISABLE_AUDIO
  return;
#endif
  Reset();
  xmp_end_player(xmpContext);
#ifndef __XTENSA__
  //TODO: Tear down SDL
#endif
}

bool WARNED_OF_PLAY_BUFFER = false;

static void fillBuffer(void *audioBuffer, size_t length) {
#ifdef DISABLE_AUDIO
  return;
#endif
  //  printf("length = %i\n", length);fflush(stdout);
  if (musicFileLoaded && ! BSoundPlayer::mAudioPaused) {
    int result = xmp_play_buffer(xmpContext, audioBuffer, length, 0);

    if (result != 0) {
      if (!WARNED_OF_PLAY_BUFFER) {
        // Something really bad happened, and audio stopped :(
        printf("xmp_play_buffer not zero (result = %i)!\n", result);fflush(stdout);
        WARNED_OF_PLAY_BUFFER = true;
      }
      memset(audioBuffer, 0, length);
    }
  }
  else {
    memset(audioBuffer, 0, length);
  }

#ifndef __XTESNA__

  if (audio.IsMuted()) {
    memset(audioBuffer, 0, length);
  }
#endif
}


#ifdef __XTENSA__

// ESP32 style timer
static void timerCallback(void *arg) {
#ifdef DISABLE_AUDIO
  return;
#endif
  //  printf("AUDIO_BUFF_SIZE %i\n", AUDIO_BUFF_SIZE); fflush(stdout);
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

  #ifndef __XTENSA__
  // Kick off SDL audio engine
  SDL_PauseAudio(0);
  #endif

  mNumberFxChannels = aNumberFxChannels;
  mNumberFxSlots = aNumberFxSlots;
}

TBool BSoundPlayer::LoadSong(BRaw *aSong) {
#ifdef DISABLE_AUDIO
  return 0;
#endif
  int loadResult = xmp_load_module_from_memory(xmpContext, aSong->mData, aSong->mSize);

  if (loadResult < 0) {
    // Sometimes XMP fails for no obvious reason. Try one more time for good measure.
    loadResult = xmp_load_module_from_memory(xmpContext, aSong->mData, aSong->mSize);
  }
//  printf("xmp_load_module_from_memory result = %i\n", loadResult); fflush(stdout);
  return loadResult == 0;
}


TBool BSoundPlayer::LoadEffect(TUint16 aResourceId, TUint8 aSlotNumber) {
#ifdef DISABLE_AUDIO
  return false;
#endif
  BRaw *effect = LoadEffectResource(aResourceId, aSlotNumber);

  int result = xmp_smix_load_sample_from_memory(xmpContext, aSlotNumber, effect->mData, effect->mSize);
  if (result < 0) {
    // Sometimes XMP fails for no obvious reason. Try one more time for good measure.
    result = xmp_smix_load_sample_from_memory(xmpContext, aSlotNumber, effect->mData, effect->mSize);;
  }
  return result == 0;
}




TUint8 sfxChannel = 0;

TBool BSoundPlayer::StopMusic() {

  // Should we test for XMP_STATE_UNLOADED, XMP_STATE_PLAYING?
  xmp_stop_module(xmpContext);
  return true;
}

TBool BSoundPlayer::Reset() {
//  xmp_end_player(xmpContext);
  PauseMusic(true);
  audio.Mute(true);
  musicFileLoaded = false;
//  printf("PlayMusic(%i);\n", aResourceId); fflush(stdout);

#ifndef __XTENSA__
  SDL_PauseAudio(1);
#endif

  //  if (xmpConte)
  xmp_set_player(xmpContext, XMP_PLAYER_VOLUME, 0);
  MuteMusic(ETrue);
  xmp_stop_module(xmpContext);

  return true;
}

TBool BSoundPlayer::SetVolume(TFloat aPercent) {

  if (xmpContext) {
    if (aPercent > 1.0f) {
      aPercent = 1.0f;
    }

    if (aPercent < 0.0f) {
      aPercent = 0;
    }

    mMusicVolume = (TUint8)(aPercent * 254);
    mEffectsVolume = mMusicVolume;

    xmp_set_player(xmpContext, XMP_PLAYER_VOLUME, mMusicVolume);
    return true;
  }

  return false;
}

TBool BSoundPlayer::SetMusicVolume(TFloat aPercent) {
#ifdef DISABLE_AUDIO
  return false;
#endif
  if (xmpContext) {
    if (aPercent > 1.0f) {
      aPercent = 1.0f;
    }
    if (aPercent < 0.0f) {
      aPercent = 0;
    }

    mMusicVolume = (TUint8)(aPercent * 254);

    xmp_set_player(xmpContext, XMP_PLAYER_VOLUME, mMusicVolume);
    return true;
  }

  return false;
}

TBool BSoundPlayer::SetEffectsVolume(TFloat aPercent) {
#ifdef DISABLE_AUDIO
  return false;
#endif
  if (xmpContext) {
    if (aPercent > 1.0f) {
      aPercent = 1.0f;
    }

    if (aPercent < 0.0f) {
      aPercent = 0;
    }

    mEffectsVolume = (TUint8)(aPercent * 254);
    xmp_set_player(xmpContext, XMP_PLAYER_SMIX_VOLUME, mEffectsVolume);
    return true;
  }

  return false;
}



TBool BSoundPlayer::PlaySfx(TInt aSoundNumber) {
#ifdef DISABLE_AUDIO
  return false;
#endif
//  printf("SFX: %i\n", aSoundNumber); fflush(stdout);
  if (! musicFileLoaded) {
    printf("%s: No Music file loaded! Cannot play sound effects!\n", __FUNCTION__);
    return false;
  }

  xmp_smix_play_sample(xmpContext, aSoundNumber, 60, mEffectsVolume, sfxChannel);

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

  Reset();

  TBool loadResult = LoadSongSlot(aResourceId);

  if (loadResult) {
    musicFileLoaded = true;
    mCurrentSongLoaded = aResourceId;
  }

  if (!musicFileLoaded) {
    printf("MUSIC LOADING FAILED!\n"); fflush(stdout);
    return EFalse;
  }

  // Every time a new song is loaded, we must load sound effects too!
  xmp_start_smix(xmpContext, mNumberFxChannels, mNumberFxSlots);

  LoadEffects();

  xmp_start_player(xmpContext, SAMPLE_RATE, 0);
  xmp_set_player(xmpContext, XMP_PLAYER_VOLUME, mMusicVolume);
  xmp_set_player(xmpContext, XMP_PLAYER_SMIX_VOLUME, mEffectsVolume); //Volume is governed globally via mEffectsVolume
  xmp_set_player(xmpContext, XMP_PLAYER_MIX, 0);


  MuteMusic(EFalse);
  PauseMusic(false);
  audio.Mute(false);
#if 0

#endif

#ifndef __XTENSA__
  SDL_PauseAudio(0);
#endif
  return ETrue;
}


