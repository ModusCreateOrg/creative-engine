#include "Audio.h"

Audio audio;
TFloat audio_volume = .5; // half way

#ifdef __XTENSA__
#define SAMPLE_RATE (22050)
#else
#define SAMPLE_RATE (44100)
#endif


/*** ODROID GO START *******/
#ifdef __XTENSA__

#define AUDIO_BUFF_SIZE 12
#define TIMER_LENGTH 50
esp_timer_create_args_t timer_args;
esp_timer_handle_t timer;

// #include "odroid_settings.h"
#define I2S_NUM (I2S_NUM_0)
#define BUILTIN_DAC_ENABLED (1)

#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include <string.h> // For memset

TFloat Audio::GetVolume(){
  return audio_volume;
}

void Audio::SetVolume(TFloat value) {
  TFloat newValue = audio_volume + value;

  if (newValue > .124f) {
    audio_volume = 0;
  }
  else {
    audio_volume = newValue;
  }

#ifndef PRODUCTION
#ifdef DEBUGME
  printf("audio_volume = %f\n", audio_volume);
#endif
#endif
}


Audio::Audio() {
//  mMuted = EFalse;
}

Audio::~Audio() {
}


void Audio::Init(TAudioDriverCallback aDriverCallback) {
#ifndef PRODUCTION
#ifdef DEBUGME
  printf("Audio::%s\n", __func__);fflush(stdout);
#endif
#endif

  // NOTE: buffer needs to be adjusted per AUDIO_SAMPLE_RATE
  i2s_config_t i2s_config;
  i2s_config.mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN);
  i2s_config.sample_rate = SAMPLE_RATE;
  i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;             //2-channels
  i2s_config.communication_format = I2S_COMM_FORMAT_PCM;
  i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;              //Interrupt level 1
  i2s_config.dma_buf_count = 48;
  i2s_config.dma_buf_len = 8;
  i2s_config.use_apll = 0; //1
#ifdef I2S_NEEDS_tx_desc_auto_clear
  i2s_config.tx_desc_auto_clear = 0;
#endif
  i2s_config.fixed_mclk = 1;

  i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);

  i2s_set_pin(I2S_NUM, NULL);
  i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);

  mAudioBuffer = (short *)AllocMem(sizeof(short) * AUDIO_BUFF_SIZE, MALLOC_CAP_8BIT); // SPI RAM
  memset(mAudioBuffer, 0, AUDIO_BUFF_SIZE);
//  audio.MuteMusic();

  //*** CREATE TIMER ***//


//  timer_args.callback = &Audio::i2sTimerCallback;
  timer_args.callback = *aDriverCallback;
  timer_args.name = "audioTimer";
//  timer_args.arg = (void *)mAudioBuffer;

  esp_timer_create(&timer_args, &timer);
  esp_timer_start_periodic(timer, TIMER_LENGTH);
}


void Audio::Terminate() {
  i2s_zero_dma_buffer(I2S_NUM);
  i2s_stop(I2S_NUM);

  i2s_start(I2S_NUM);


  esp_err_t err = rtc_gpio_init(GPIO_NUM_25);
  err = rtc_gpio_init(GPIO_NUM_26);

  if (err != ESP_OK){
    abort();
  }

  err = rtc_gpio_set_direction(GPIO_NUM_25, RTC_GPIO_MODE_OUTPUT_ONLY);
  err = rtc_gpio_set_direction(GPIO_NUM_26, RTC_GPIO_MODE_OUTPUT_ONLY);

  if (err != ESP_OK) {
    abort();
  }

  err = rtc_gpio_set_level(GPIO_NUM_25, 0);
  err = rtc_gpio_set_level(GPIO_NUM_26, 0);
  if (err != ESP_OK) {
    abort();
  }
}


void Audio::Submit(TInt16* stereomAudioBuffer, int frameCount) {
  TInt16 currentAudioSampleCount = frameCount * 2;

  // Convert for built in DAC
  for (TInt16 i = 0; i < currentAudioSampleCount; i += 2){
    int32_t dac0;
    int32_t dac1;

    if (audio_volume == 0.0f || mMuted) {
      // Disable amplifier
      dac0 = 0;
      dac1 = 0;
    }
    else {
      // Down mix stero to mono
      int32_t sample = stereomAudioBuffer[i];
      sample += stereomAudioBuffer[i + 1];
      sample >>= 1;

      // Normalize
      const TFloat sn = (TFloat)sample / 0x8000;

      // Scale
      const int magnitude = 127 + 127;
      const TFloat range = magnitude  * sn * audio_volume;

      // Convert to differential output
      if (range > 127) {
        dac1 = (range - 127);
        dac0 = 127;
      }
      else if (range < -127) {
        dac1  = (range + 127);
        dac0 = -127;
      }
      else{
        dac1 = 0;
        dac0 = range;
      }

      dac0 += 0x80;
      dac1 = 0x80 - dac1;

      dac0 <<= 8;
      dac1 <<= 8;
    }

    stereomAudioBuffer[i] = (int16_t)dac1;
    stereomAudioBuffer[i + 1] = (int16_t)dac0;
  }


  int len = currentAudioSampleCount * sizeof(int16_t);
  // this replaces i2s_write_bytes
  size_t count;
  i2s_write(I2S_NUM, (const char *)stereomAudioBuffer, len, &count, portMAX_DELAY);
//  int count = i2s_write_bytes(I2S_NUM, (const char *)stereomAudioBuffer, len, portMAX_DELAY);

  if (count != len)   {
#ifndef PRODUCTION
#ifdef DEBUGME
    printf("i2s_write_bytes: count (%d) != len (%d)\n", count, len);
#endif
#endif
    abort();
  }
}


/**** END ODROID GO ***/
#else
/*** START Mac/Linux ***/

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>


Audio::Audio() {
}

Audio::~Audio() {
  SDL_CloseAudio();
}



void Audio::Init(TAudioDriverCallback aDriverCallback) {
  SDL_AudioSpec audioSpec;
#ifndef PRODUCTION
  printf("Initializing SDL2 Audio\n");
#endif
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
#ifndef PRODUCTION
    fprintf(stderr, "sdl: can't initialize: %s\n", SDL_GetError());
#endif
    return;
  }

  audioSpec.freq = SAMPLE_RATE;
  audioSpec.format = AUDIO_S16;
  audioSpec.channels = 2;
#ifdef __arm__
  //Optimized for the RPie 3B+
  audioSpec.samples = 1024;
  audioSpec.size = 512;
#else
  audioSpec.samples = 512;
  audioSpec.size = 100;
#endif
  audioSpec.callback = aDriverCallback;

  if (SDL_OpenAudio(&audioSpec, nullptr) < 0) {
#ifndef PRODUCTION
    fprintf(stderr, "%s\n", SDL_GetError());
#endif
  }

#ifndef PRODUCTION
 printf("AUDIO SPEC: samples: %i | size: %i\n", audioSpec.samples, audioSpec.size);
#endif

}

void Audio::SetVolume(TFloat value) {

}
TFloat Audio::GetVolume() {
  return 0.0;
}


/**** END Mac/Linux ****/
#endif





