//
// Created by Michael Schwartz on 9/4/18.
//

#include "Controls.h"

Controls gControls;

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

  state |= gpio_get_level(ODROID_GAMEPAD_IO_SELECT) ? 0 : BUTTON3;
  state |= gpio_get_level(ODROID_GAMEPAD_IO_START) ? 0 : BUTTON4;

  state |= gpio_get_level(ODROID_GAMEPAD_IO_A) ? 0 : BUTTONA;
  state |= gpio_get_level(ODROID_GAMEPAD_IO_B) ? 0 : BUTTONB;

  state |= gpio_get_level(ODROID_GAMEPAD_IO_MENU) ? 0 : BUTTON1;
  state |= gpio_get_level(ODROID_GAMEPAD_IO_VOLUME) ? 0 : BUTTON2;

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

Controls::Controls() {
#ifdef CONTROLLER_SUPPORT
  SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC);

  haptic = ENull;
  ctrl = ENull;

  for (TInt i = 0; i < SDL_NumJoysticks(); ++i) {
    if (!SDL_IsGameController(i)) {
      continue;
    }
    ctrl = SDL_GameControllerOpen(i);
    haptic = SDL_HapticOpenFromJoystick(SDL_GameControllerGetJoystick(ctrl));
#ifndef PRODUCTION
#if (defined(__XTENSA__) && defined(DEBUGME)) || !defined(__XTENSA__)
      SDL_Log("Found a compatible controller named \'%s\'", SDL_GameControllerNameForIndex(i));
      SDL_Log("Controller %i is mapped as \"%s\".", i, SDL_GameControllerMapping(ctrl));
#endif
#endif
  }
#endif
  Reset();
}

Controls::~Controls() {
#ifdef CONTROLLER_SUPPORT
  if (haptic) {
    SDL_HapticClose(haptic);
  }

  SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC);
#endif
}

void Controls::Rumble(TFloat aStrength, TInt aTime) {
#ifdef CONTROLLER_SUPPORT
  if (haptic == ENull || SDL_NumJoysticks() == 0) {
    return;
  }

  // Initialize simple rumble
  SDL_HapticRumbleInit(haptic);
  SDL_HapticRumblePlay(haptic, aStrength, aTime);
#endif
}

TBool Controls::Poll() {
  SDL_Event e;

  while (SDL_PollEvent(&e)) {
    TUint16 keys = bKeys;
    if (e.type == SDL_QUIT) {
      keys |= BUTTONQ;
    }

    // Controllers
#ifdef CONTROLLER_SUPPORT
    if (SDL_NumJoysticks() > 0) {
      if (SDL_GameControllerGetAxis(ctrl, SDL_CONTROLLER_AXIS_LEFTY) > CONTROLLER_AXIS_MIN || SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) {
        keys |= JOYDOWN;
      } else {
        keys &= ~JOYDOWN;
      }
      if (SDL_GameControllerGetAxis(ctrl, SDL_CONTROLLER_AXIS_LEFTY) < -CONTROLLER_AXIS_MIN || SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_DPAD_UP)) {
        keys |= JOYUP;
      } else {
        keys &= ~JOYUP;
      }
      if (SDL_GameControllerGetAxis(ctrl, SDL_CONTROLLER_AXIS_LEFTX) > CONTROLLER_AXIS_MIN || SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        keys |= JOYRIGHT;
      } else {
        keys &= ~JOYRIGHT;
      }
      if (SDL_GameControllerGetAxis(ctrl, SDL_CONTROLLER_AXIS_LEFTX) < -CONTROLLER_AXIS_MIN || SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) {
        keys |= JOYLEFT;
      } else {
        keys &= ~JOYLEFT;
      }

      if (e.type == SDL_CONTROLLERBUTTONDOWN) {
        switch (e.cbutton.button) {
          case SDL_CONTROLLER_BUTTON_A:
            keys |= BUTTONB;
            break;
          case SDL_CONTROLLER_BUTTON_B:
            keys |= BUTTONA;
            break;
          case SDL_CONTROLLER_BUTTON_X:
            keys |= BUTTON_MENU;
            break;
          case SDL_CONTROLLER_BUTTON_Y:
            keys |= BUTTON_SOUND;
            break;
          case SDL_CONTROLLER_BUTTON_BACK:
            keys |= BUTTON_SELECT;
            break;
          case SDL_CONTROLLER_BUTTON_START:
            keys |= BUTTON_START;
            break;
        }
      }
      if (e.type == SDL_CONTROLLERBUTTONUP) {
        switch (e.cbutton.button) {
          case SDL_CONTROLLER_BUTTON_A:
            keys &= ~BUTTONB;
            break;
          case SDL_CONTROLLER_BUTTON_B:
            keys &= ~BUTTONA;
            break;
          case SDL_CONTROLLER_BUTTON_X:
            keys &= ~BUTTON_MENU;
            break;
          case SDL_CONTROLLER_BUTTON_Y:
            keys &= ~BUTTON_SOUND;
            break;
          case SDL_CONTROLLER_BUTTON_BACK:
            keys &= ~BUTTON_SELECT;
            break;
          case SDL_CONTROLLER_BUTTON_START:
            keys &= ~BUTTON_START;
            break;
        }
      }
    }
#endif

    // Keyboard
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
        case SDL_SCANCODE_RETURN:
          keys |= BUTTON4;
          break;
        case SDL_SCANCODE_RALT:
        case SDL_SCANCODE_LCTRL:
        case SDL_SCANCODE_X:
          keys |= BUTTONA;
          break;
        case SDL_SCANCODE_SPACE:
        case SDL_SCANCODE_Z:
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
     if (e.type == SDL_KEYUP) {
      switch (e.key.keysym.scancode) {
        // QUIT button, will never be set on target
        case SDL_SCANCODE_Q:
          keys &= ~BUTTONQ;
          break;
        case SDL_SCANCODE_1:
          keys &= ~BUTTON1;
          break;
        case SDL_SCANCODE_2:
          keys &= ~BUTTON2;
          break;
        case SDL_SCANCODE_3:
          keys &= ~BUTTON3;
          break;
        case SDL_SCANCODE_4:
        case SDL_SCANCODE_RETURN:
          keys &= ~BUTTON4;
          break;
        case SDL_SCANCODE_RALT:
        case SDL_SCANCODE_LCTRL:
        case SDL_SCANCODE_Z:
          keys &= ~BUTTONB;
          break;
        case SDL_SCANCODE_SPACE:
        case SDL_SCANCODE_X:
        case SDL_SCANCODE_RCTRL:
        case SDL_SCANCODE_LALT:
          keys &= ~BUTTONA;
          break;
        case SDL_SCANCODE_K:
        case SDL_SCANCODE_UP:
          keys &= ~JOYUP;
          break;
        case SDL_SCANCODE_J:
        case SDL_SCANCODE_DOWN:
          keys &= ~JOYDOWN;
          break;
        case SDL_SCANCODE_H:
        case SDL_SCANCODE_LEFT:
          keys &= ~JOYLEFT;
          break;
        case SDL_SCANCODE_L:
        case SDL_SCANCODE_RIGHT:
          keys &= ~JOYRIGHT;
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
