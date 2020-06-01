//
// Created by Jesus Garcia on 11/8/18.
//

#ifndef GENUS_BSOUNDPLAYER_H
#define GENUS_BSOUNDPLAYER_H

#include <BTypes.h>
#include <BBase.h>
#include <BResourceManager.h>

#include <SDL.h>
#include <SDL_audio.h>
#include <Audio_Mixer.h>

/**
 * Abstract sound player class.
 *
 * Each game will inherit to make GSoundPlayer and implement
 * the pure virtual methods.
 */

typedef void (*TAudioDriverCallback)(void *userdata, Uint8 *stream, int len);

#ifndef __DINGUX__
typedef SDL_AudioCallback TAudioDriverCallback;

#endif

//#ifdef __MODUS_TARGET_SDL2_AUDIO__
#define SAMPLE_RATE (44100)
//#endif
//#define SAMPLE_RATE (22500)



struct SongSlot {
  BRaw *mRaw;
  TUint16 mResourceNumber;
  TUint16 mSlotNumber;
};

class BSoundEngine : public BBase {

public:
  TUint8 mMusicVolume{};
  TUint8 mEffectsVolume{};
  TInt16 mCurrentSongLoaded{};
  TUint8 mNumberFxChannels{};

  TUint8 mMaxSongs{};
  TUint8 mMaxEffects{};
  TUint8 mNumSfxChannels{};

  TFloat mAudioVolume{};
  Mixer_Chunk **mEffects{};
  TUint8  mNumEffects{};
  short *mAudioBuffer;
  TBool mMuted;
  TBool mAudioPaused;

  BSoundEngine();
  ~BSoundEngine();



  void MuteMusic(TBool aMuted = ETrue) {
    mMuted = aMuted;
  }

  TBool Mute(TBool aMuted) {
    mMuted = aMuted;
    return ETrue;
  }

  TBool PauseMusic(TBool aPaused = ETrue) {
    mAudioPaused = aPaused;
    return ETrue;
  }

  TBool IsMuted() {
    return mMuted;
  }


  void InitAudioEngine(TUint8 aNumSfxChannels, TUint8 aNumSfxFiles);
  TBool SetVolume(TFloat aPercent);
  TBool SetMusicVolume(TFloat aPercent);
  TBool SetEffectsVolume(TFloat aPercent);
  TBool PlaySfx(TInt aSoundNumber, TInt8 aChannel);
  TBool PlayMusic(BRaw *aRawFile, TInt16 aResourceId);

//  BRaw *LoadEffectResource(TUint16 aResourceId, TInt16 aSlotNumber);

  TBool LoadEffect(TUint8 aSfxIndex, TUint16 aResourceId, TUint8 aResourceSlot);

//  TBool LoadEffects() { };

  TBool LoadSong(BRaw *aSong);
//  TBool FindRawSongFileById(TInt16 aSongNumber) { };/**/
  TBool StopMusic();
  TBool Reset();

//    virtual void SetVolume(TFloat value) = 0;

  TFloat GetVolume() {
    return mAudioVolume;
  }




};

extern BSoundEngine soundEngine;



#endif //GENUS_BSOUNDPLAYER_H
