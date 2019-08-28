#ifndef AUDIO_H
#define AUDIO_H


// ODROID GO
#ifdef __XTENSA__
#include "OdroidAudio.h"
class Audio : public OdroidAudio {};
#endif

#ifdef __DINGUX__
#include "LDKAudio.h"
class Audio : public SDL2Audio {};
#endif

// Desktop / Linux / Raspberry Pi
#ifdef __MODUS_TARGET_SDL2_AUDIO__
#include "SDL2Audio.h"
class Audio : public SDL2Audio {};
#endif


extern Audio audio;


#endif
