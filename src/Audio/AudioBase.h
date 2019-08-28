#ifndef GENUS_AUDIOBASE_H
#define GENUS_AUDIOBASE_H

#include "BTypes.h"
#include "BBase.h"


class AudioBase : public BBase {
protected:
  TBool mMuted;
  TFloat mAudioVolume;

public:
  AudioBase() {
    mAudioVolume = .5;
    mMuted = ETrue;
    mAudioBuffer = ENull;
  }

   ~AudioBase() = default;

  virtual void SetVolume(TFloat value) = 0;

  TFloat GetVolume() {
    return mAudioVolume;
  }


  virtual void Terminate() = 0;

  void Mute(TBool aMuted) {
    mMuted = aMuted;
  }

  TBool IsMuted() {
    return mMuted;
  }

public:
  short *mAudioBuffer;

};



#endif //GENUS_AUDIOBASE_H

