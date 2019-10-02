#ifdef __MODUS_TARGET_DESKTOP_CONTROLS__

#include "DesktopControls.h"

#ifdef CONTROLLER_SUPPORT
#include <SDL.h>
#endif


DesktopControls::DesktopControls() : ControlsBase() {
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
    break;

#ifndef PRODUCTION
    SDL_Log("Found a compatible controller named \'%s\'", SDL_GameControllerNameForIndex(i));
    SDL_Log("Controller %i is mapped as \"%s\".", i, SDL_GameControllerMapping(ctrl));
#endif

  }

#endif
}

DesktopControls::~DesktopControls() {
#ifdef CONTROLLER_SUPPORT
  if (haptic) {
    SDL_HapticClose(haptic);
  }

  SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC);
#endif
}

void DesktopControls::Rumble(TFloat aStrength, TInt aTime) {
#ifdef CONTROLLER_SUPPORT
  if (haptic == ENull || SDL_NumJoysticks() == 0) {
    return;
  }

  // Initialize simple rumble
  SDL_HapticRumbleInit(haptic);
  SDL_HapticRumblePlay(haptic, aStrength, aTime);
#endif
}

TBool DesktopControls::Poll()  {
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
        keys &= TUint16(~JOYDOWN);
      }
      if (SDL_GameControllerGetAxis(ctrl, SDL_CONTROLLER_AXIS_LEFTY) < -CONTROLLER_AXIS_MIN || SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_DPAD_UP)) {
        keys |= JOYUP;
      } else {
        keys &= TUint16(~JOYUP);
      }
      if (SDL_GameControllerGetAxis(ctrl, SDL_CONTROLLER_AXIS_LEFTX) > CONTROLLER_AXIS_MIN || SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        keys |= JOYRIGHT;
      } else {
        keys &= TUint16(~JOYRIGHT);
      }
      if (SDL_GameControllerGetAxis(ctrl, SDL_CONTROLLER_AXIS_LEFTX) < -CONTROLLER_AXIS_MIN || SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) {
        keys |= JOYLEFT;
      } else {
        keys &= TUint16(~JOYLEFT);
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
            keys &= TUint16(~BUTTONB);
            break;
          case SDL_CONTROLLER_BUTTON_B:
            keys &= TUint16(~~BUTTONA);
            break;
          case SDL_CONTROLLER_BUTTON_X:
            keys &= TUint16(~BUTTON_MENU);
            break;
          case SDL_CONTROLLER_BUTTON_Y:
            keys &= TUint16(~BUTTON_SOUND);
            break;
          case SDL_CONTROLLER_BUTTON_BACK:
            keys &= TUint16(~BUTTON_SELECT);
            break;
          case SDL_CONTROLLER_BUTTON_START:
            keys &= TUint16(~BUTTON_START);
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
        case SDL_SCANCODE_LALT:
        case SDL_SCANCODE_X:
        case SDL_SCANCODE_F:
          keys |= BUTTONA;
          break;
//        case SDL_SCANCODE_SPACE:
        case SDL_SCANCODE_LCTRL:
        case SDL_SCANCODE_Z:
        case SDL_SCANCODE_D:
          keys |= BUTTONB;
          break;
        case SDL_SCANCODE_S:
          keys |= BUTTONX;
          break;
        case SDL_SCANCODE_A:
          keys |= BUTTONY;
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
        case SDL_SCANCODE_TAB:
          keys |= BUTTONL;
          break;
        case SDL_SCANCODE_BACKSPACE:
          keys |= BUTTONR;
          break;
        default:
          break;
      }
    }
    if (e.type == SDL_KEYUP) {
      switch (e.key.keysym.scancode) {
        // QUIT button, will never be set on target
        case SDL_SCANCODE_Q:
          keys &= TUint16(~BUTTONQ);
          break;
        case SDL_SCANCODE_1:
          keys &= TUint16(~BUTTON1);
          break;
        case SDL_SCANCODE_2:
          keys &= TUint16(~BUTTON2);
          break;
        case SDL_SCANCODE_3:
          keys &= TUint16(~BUTTON3);
          break;
        case SDL_SCANCODE_4:
        case SDL_SCANCODE_RETURN:
          keys &= TUint16(~BUTTON4);
          break;
        case SDL_SCANCODE_LALT:
        case SDL_SCANCODE_X:
        case SDL_SCANCODE_F:
          keys &= TUint16(~BUTTONA);
          break;
//        case SDL_SCANCODE_SPACE:
        case SDL_SCANCODE_LCTRL:
        case SDL_SCANCODE_Z:
        case SDL_SCANCODE_D:
          keys &= TUint16(~BUTTONB);
          break;
        case SDL_SCANCODE_S:
          keys &= TUint16(~BUTTONX);
          break;
        case SDL_SCANCODE_A:
          keys &= TUint16(~BUTTONY);
          break;
        case SDL_SCANCODE_K:
        case SDL_SCANCODE_UP:
          keys &= TUint16(~JOYUP);
          break;
        case SDL_SCANCODE_J:
        case SDL_SCANCODE_DOWN:
          keys &= TUint16(~JOYDOWN);
          break;
        case SDL_SCANCODE_H:
        case SDL_SCANCODE_LEFT:
          keys &= TUint16(~JOYLEFT);
          break;
        case SDL_SCANCODE_L:
        case SDL_SCANCODE_RIGHT:
          keys &= TUint16(~JOYRIGHT);
          break;
        case SDL_SCANCODE_TAB:
          keys &= TUint16(~BUTTONL);
          break;
        case SDL_SCANCODE_BACKSPACE:
          keys &= TUint16(~BUTTONR);
          break;
        default:
          break;
      }
    }
    // cKeys are journaled if journaling is on!
    dKeys |= TUint16(TUint16(keys) ^ TUint16(TUint16(cKeys) & TUint16(keys)));
    cKeys        = keys;
    bKeys        = keys;
  }

  return false;
}

#endif