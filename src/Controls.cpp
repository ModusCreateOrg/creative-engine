//
// Created by Michael Schwartz on 9/4/18.
//

#include "Controls.h"

Controls controls;

#ifdef __XTENSA__

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_adc_cal.h"
#include "freertos/FreeRTOS.h"
#include <driver/adc.h>

#define ODROID_GAMEPAD_IO_X ADC1_CHANNEL_6
#define ODROID_GAMEPAD_IO_Y ADC1_CHANNEL_7
#define ODROID_GAMEPAD_IO_SELECT GPIO_NUM_27
#define ODROID_GAMEPAD_IO_START GPIO_NUM_39
#define ODROID_GAMEPAD_IO_A GPIO_NUM_32
#define ODROID_GAMEPAD_IO_B GPIO_NUM_33
#define ODROID_GAMEPAD_IO_MENU GPIO_NUM_13
#define ODROID_GAMEPAD_IO_VOLUME GPIO_NUM_0

static TUint16 buttonsState() {
  TUint16 state = 0;

  TInt joyX = adc1_get_raw(ODROID_GAMEPAD_IO_X),
      joyY = adc1_get_raw(ODROID_GAMEPAD_IO_Y);

  if (joyX > 2048 + 1024) {
    state |= JOYLEFT;
  }
  else if (joyX > 1024) {
    state |= JOYRIGHT;
  }
  if (joyY > 2048 + 1024) {
    state |= JOYUP;
  }
  else if (joyY > 1024) {
    state |= JOYDOWN;
  }

  state |= gpio_get_level(ODROID_GAMEPAD_IO_SELECT) ? 0 : BUTTON1;
  state |= gpio_get_level(ODROID_GAMEPAD_IO_START) ? 0 : BUTTON2;

  state |= gpio_get_level(ODROID_GAMEPAD_IO_A) ? 0 : BUTTONA;
  state |= gpio_get_level(ODROID_GAMEPAD_IO_B) ? 0 : BUTTONB;

  state |= gpio_get_level(ODROID_GAMEPAD_IO_MENU) ? 0 : BUTTON3;
  state |= gpio_get_level(ODROID_GAMEPAD_IO_VOLUME) ? 0 : BUTTON4;

  return state;
}

Controls::Controls() {
  Reset();
  gpio_set_direction(ODROID_GAMEPAD_IO_SELECT, GPIO_MODE_INPUT);
  gpio_set_pull_mode(ODROID_GAMEPAD_IO_SELECT, GPIO_PULLUP_ONLY);

  gpio_set_direction(ODROID_GAMEPAD_IO_START, GPIO_MODE_INPUT);

  gpio_set_direction(ODROID_GAMEPAD_IO_A, GPIO_MODE_INPUT);
  gpio_set_pull_mode(ODROID_GAMEPAD_IO_A, GPIO_PULLUP_ONLY);

  gpio_set_direction(ODROID_GAMEPAD_IO_B, GPIO_MODE_INPUT);
  gpio_set_pull_mode(ODROID_GAMEPAD_IO_B, GPIO_PULLUP_ONLY);

  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ODROID_GAMEPAD_IO_X, ADC_ATTEN_11db);
  adc1_config_channel_atten(ODROID_GAMEPAD_IO_Y, ADC_ATTEN_11db);

  gpio_set_direction(ODROID_GAMEPAD_IO_MENU, GPIO_MODE_INPUT);
  gpio_set_pull_mode(ODROID_GAMEPAD_IO_MENU, GPIO_PULLUP_ONLY);

  gpio_set_direction(ODROID_GAMEPAD_IO_VOLUME, GPIO_MODE_INPUT);
}

Controls::~Controls() {
  //
}

TBool Controls::Poll() {
  TUint16 buttons = buttonsState();
  dKeys = (buttons ^ cKeys) & buttons;
  cKeys = buttons;
  bKeys = buttons;
  return ETrue;
}
#else

#include <SDL2/SDL.h>

Controls::Controls() {
  Reset();
}

Controls::~Controls() {
  //
}

TBool Controls::Poll() {
  SDL_Event e;

  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      return false;
    }
    TUint16 keys = 0;
    if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.scancode) {
        // QUIT button, will never be set on target
        case SDL_SCANCODE_Q:
          keys |= BUTTONQ;
          break;
        case SDL_SCANCODE_1:
          keys |= BUTTON1;
          break;
        case SDL_SCANCODE_2:
          keys |= BUTTON2;
          break;
        case SDL_SCANCODE_3:
          keys |= BUTTON3;
          break;
        case SDL_SCANCODE_4:
          keys |= BUTTON4;
          break;
        case SDL_SCANCODE_RALT:
        case SDL_SCANCODE_LCTRL:
        case SDL_SCANCODE_Z:
          keys |= BUTTONA;
          break;
        case SDL_SCANCODE_X:
        case SDL_SCANCODE_RCTRL:
        case SDL_SCANCODE_LALT:
          keys |= BUTTONB;
          break;
        case SDL_SCANCODE_K:
        case SDL_SCANCODE_UP:
          keys |= JOYUP;
          break;
        case SDL_SCANCODE_J:
        case SDL_SCANCODE_DOWN:
          keys |= JOYDOWN;
          break;
        case SDL_SCANCODE_H:
        case SDL_SCANCODE_LEFT:
          keys |= JOYLEFT;
          break;
        case SDL_SCANCODE_L:
        case SDL_SCANCODE_RIGHT:
          keys |= JOYRIGHT;
          break;
        default:
          break;
      }
    }
    // cKeys are journaled if journaling is on!
    dKeys |= (keys ^ cKeys) & keys;
    cKeys        = keys;
    bKeys        = keys;
//    if (e.type == SDL_MOUSEBUTTONDOWN) {
//      quit = true;
//    }
  }
  return false;
}
#endif
