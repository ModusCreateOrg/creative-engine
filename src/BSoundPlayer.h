//
// Created by Jesus Garcia on 11/8/18.
//

#ifndef GENUS_BSOUNDPLAYER_H
#define GENUS_BSOUNDPLAYER_H

#include <Audio.h>
#include <BTypes.h>
#include <BBase.h>
#include <BResourceManager.h>


/**
 * Abstract sound player class.
 *
 * Each game will inherit to make GSoundPlayer and implement
 * the pure virtual methods.
 */


struct SongSlot {
  BRaw *mRaw;
  TUint16 mResourceNumber;
  TUint16 mSlotNumber;
};

class BSoundPlayer : public BBase {

public:
  BSoundPlayer();
  ~BSoundPlayer();

public: static TBool mAudioPaused;


public:
  TUint8 mMusicVolume;
  TUint8 mEffectsVolume;
  TBool mMuted;
  TInt16 mCurrentSongLoaded;
  TUint8 mNumberFxChannels;
  TUint8 mNumberFxSlots;

  TUint8 mMaxSongs;
  TUint8 mMaxEffects;

  SongSlot *mSongSlots; // Used to store number of slots




public:
  void Init(TUint8 aNumberFxChannels, TUint8 aNumberFxSlots);


//   // set system volume
  TBool SetVolume(TFloat aPercent);
  TBool SetMusicVolume(TFloat aPercent);
  TBool SetEffectsVolume(TFloat aPercent);
//   // mute or unute sounds & music (MASTER mute)
//   virtual TBool Mute(TBool aMuted = ETrue) = 0;
// public:
//   // play a sound, overriding an existing sound with lower priority (if necessary)
//   // sound will loop back to start if flag is set
  TBool PlaySfx(TInt aSoundNumber);
//   // stop a currently playing sound
//   virtual TBool StopSound(TInt aSoundNumber) = 0;
//   // mute only sound effects (music will continue to be heard)
//   virtual TBool MuteSounds(TBool aMuted = ETrue) = 0;
//   // stop all sounds (but not music)
//   virtual TBool StopSounds() = 0;
// public:
//   // play a song, track will loop back to start if flag is set
//   // if a score is already playing, it will be stopped first
  TBool PlayMusic(TInt16 aResourceId);

  virtual BRaw *LoadEffectResource(TUint16 aResourceId, TInt16 aSlotNumber) = 0;

  TBool LoadEffect(TUint16 aResourceId, TUint8 aSlotNumber);

  virtual TBool LoadEffects() = 0;

  TBool LoadSong(BRaw *aSong);

  virtual TBool LoadSongSlot(TInt16 aSongNumber) = 0;

//   //  stop playing music
  TBool StopMusic();

//   // toggle music paused/playing

//   // toggle music muted (will not mute sound effects)
// public:
//   // reset music player, stop all sounds and music
  TBool Reset();

  TBool MuteMusic(TBool aMuted = ETrue) {
    mMuted = aMuted;
    audio.Mute(mMuted);
    return true;
  }
  TBool PauseMusic(TBool aPaused = ETrue) {
    BSoundPlayer::mAudioPaused = aPaused;
    return true;
  }

};




#endif //GENUS_BSOUNDPLAYER_H
