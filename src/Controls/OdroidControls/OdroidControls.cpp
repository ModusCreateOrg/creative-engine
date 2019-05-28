#ifdef __XTENSA__
#include "OdroidControls.h"



#define ODROID_GAMEPAD_IO_X ADC1_CHANNEL_6
#define ODROID_GAMEPAD_IO_Y ADC1_CHANNEL_7
#define ODROID_GAMEPAD_IO_SELECT GPIO_NUM_27
#define ODROID_GAMEPAD_IO_START GPIO_NUM_39
#define ODROID_GAMEPAD_IO_A GPIO_NUM_32
#define ODROID_GAMEPAD_IO_B GPIO_NUM_33
#define ODROID_GAMEPAD_IO_MENU GPIO_NUM_13
#define ODROID_GAMEPAD_IO_VOLUME GPIO_NUM_0


OdroidControls::OdroidControls() : ControlsBase() {
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


TBool OdroidControls::Poll() {
  TUint16 buttons = 0;

  TInt joyX = adc1_get_raw(ODROID_GAMEPAD_IO_X),
       joyY = adc1_get_raw(ODROID_GAMEPAD_IO_Y);

  if (joyX > 2048 + 1024) {
    buttons |= JOYLEFT;
  }
  else if (joyX > 1024) {
    buttons |= JOYRIGHT;
  }
  if (joyY > 2048 + 1024) {
    buttons |= JOYUP;
  }
  else if (joyY > 1024) {
    buttons |= JOYDOWN;
  }

  buttons |= gpio_get_level(ODROID_GAMEPAD_IO_SELECT) ? 0 : BUTTON3;
  buttons |= gpio_get_level(ODROID_GAMEPAD_IO_START) ? 0 : BUTTON4;

  buttons |= gpio_get_level(ODROID_GAMEPAD_IO_A) ? 0 : BUTTONA;
  buttons |= gpio_get_level(ODROID_GAMEPAD_IO_B) ? 0 : BUTTONB;

  buttons |= gpio_get_level(ODROID_GAMEPAD_IO_MENU) ? 0 : BUTTON1;
  buttons |= gpio_get_level(ODROID_GAMEPAD_IO_VOLUME) ? 0 : BUTTON2;

  dKeys = (buttons ^ cKeys) & buttons;
  cKeys = buttons;
  bKeys = buttons;
  return ETrue;
}



#endif