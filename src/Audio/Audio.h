#ifndef AUDIO_H
#define AUDIO_H


// ODROID GO
#ifdef __XTENSA__
#include "OdroidAudio.h"
class Audio : public OdroidAudio {};
#endif

// Desktop / Linux / Raspberry Pi
#ifndef __XTENSA__
#include "SDL2Audio.h"
class Audio : public SDL2Audio {};
#endif


extern Audio audio;


#endif
