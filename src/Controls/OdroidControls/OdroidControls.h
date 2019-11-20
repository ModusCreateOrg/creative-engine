#ifdef __XTENSA__

#ifndef ODROIDCONTROLS_H
#define ODROIDCONTROLS_H


#include "Controls.h"

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_adc_cal.h"
#include "freertos/FreeRTOS.h"
#include <driver/adc.h>

class OdroidControls : public Controls {

public:
  OdroidControls();

  ~OdroidControls() = default;
  TBool Poll() override;
};


#endif //ODROIDCONTROLS_H

#endif // __MODUS_TARGET_ODROID__
