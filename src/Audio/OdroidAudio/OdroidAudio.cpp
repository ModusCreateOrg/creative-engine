#ifdef __XTENSA__

#include "OdroidAudio.h"


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



void OdroidAudio::Init(TAudioDriverCallback aDriverCallback) {
#ifdef DEBUGME
  printf("OdroidAudio::%s\n", __func__);fflush(stdout);
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

  //*** CREATE TIMER ***//
  timer_args.callback = *aDriverCallback;
  timer_args.name = "audioTimer";

  esp_timer_create(&timer_args, &timer);
  esp_timer_start_periodic(timer, TIMER_LENGTH);
}


void OdroidAudio::Terminate() {
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


void OdroidAudio::Submit(TInt16* stereomAudioBuffer, TInt frameCount) {
  TInt16 currentAudioSampleCount = frameCount * 2;

  // Convert for built in DAC
  for (TInt16 i = 0; i < currentAudioSampleCount; i += 2){
    TInt32 dac0;
    TInt32 dac1;

    if (mAudioVolume == 0.0f || mMuted) {
      // Disable amplifier
      dac0 = 0;
      dac1 = 0;
    }
    else {
      // Down mix stero to mono
      TInt32 sample = stereomAudioBuffer[i];
      sample += stereomAudioBuffer[i + 1];
      sample >>= 1;

      // Normalize
      const TFloat sn = (TFloat)sample / 0x8000;

      // Scale
      const TInt magnitude = 127 + 127;
      const TFloat range = magnitude  * sn * mAudioVolume;

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

    stereomAudioBuffer[i] = (TInt16)dac1;
    stereomAudioBuffer[i + 1] = (TInt16)dac0;
  }


  TInt len = currentAudioSampleCount * sizeof(TInt16);
  // this replaces i2s_write_bytes
  size_t count;
  i2s_write(I2S_NUM, (const char *)stereomAudioBuffer, len, &count, portMAX_DELAY);
//  int count = i2s_write_bytes(I2S_NUM, (const char *)stereomAudioBuffer, len, portMAX_DELAY);

  if (count != len)   {
    abort();
  }
}

#endif