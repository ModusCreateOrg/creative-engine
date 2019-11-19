#ifdef __XTENSA__
#ifndef GENUS_ODROIDSTORE_H
#define GENUS_ODROIDSTORE_H


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <driver/gpio.h>

#include "BStoreBase.h"

class OdroidStore : public BStoreBase {
public:
  explicit OdroidStore(const char *aStoreName) : BStoreBase(aStoreName){
    Initialize();
  }

  void Initialize() OVERRIDE {
    if (!mInitialized) {
      esp_err_t err = nvs_flash_init();
      if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
      }
      ESP_ERROR_CHECK(err);
      mInitialized = ETrue;
    }
  }

  TBool Get(const char *aKey, void *aValue, TUint32 aSize) override {
    nvs_handle my_handle;
    esp_err_t err;

    err = nvs_open(mStoreName, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
      return EFalse;
    }

    TUint32 size = 0;
    err = nvs_get_blob(my_handle, aKey, NULL, &size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
      return EFalse;
    }
    if (size != aSize) {
      return EFalse;
    }

    err = nvs_get_blob(my_handle, aKey, aValue, &size);
    nvs_close(my_handle);
    return err == ESP_OK;
  }

  TBool Set(const char *aKey, void *aValue, TUint32 aSize) override {
    nvs_handle my_handle;
    esp_err_t err;

    err = nvs_open(mStoreName, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
#ifndef PRODUCTION
      #if (defined(__XTENSA__) && defined(DEBUGME)) || !defined(__XTENSA__)
    printf("Set: nvs_open failed, returned %d\n", err);
#endif
#endif
      return EFalse;
    }

    err = nvs_set_blob(my_handle, aKey, aValue, aSize);
    if (err == ESP_OK) {
      err = nvs_commit(my_handle);
    }
    else {
#ifndef PRODUCTION
      #if (defined(__XTENSA__) && defined(DEBUGME)) || !defined(__XTENSA__)
    printf("Set: nvs_set_blob failed, returned %d\n", err);
#endif
#endif
    }
    nvs_close(my_handle);
    return err == ESP_OK;
  }

};



#endif //GENUS_ODROIDSTORE_H

#endif
