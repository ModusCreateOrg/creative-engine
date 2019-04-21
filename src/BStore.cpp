#include "BStore.h"
#include <string.h>
#include <stdlib.h>
#include <string.h>


#if defined(__APPLE__) || defined(__linux__)
#include <dirent.h>
#include <sys/stat.h>
#endif

TBool BStore::mInitialized = EFalse;

#ifdef __XTENSA__
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <driver/gpio.h>

BStore::BStore(const char *aStoreName) : mStoreName(strdup(aStoreName)){
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

BStore::~BStore() {
  FreeMem((TAny*)mStoreName);
}

TBool BStore::Get(const char *aKey, void *aValue, TUint32 aSize) {
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
#ifndef PRODUCTION
#if (defined(__XTENSA__) && defined(DEBUGME)) || !defined(__XTENSA__)
    printf("BStore::Get aSize=%d, actual=%d\n", size, aSize);
#endif
#endif
    return EFalse;
  }

  err = nvs_get_blob(my_handle, aKey, aValue, &size);
  nvs_close(my_handle);
  return err == ESP_OK;
}

TBool BStore::Set(const char *aKey, void *aValue, TUint32 aSize) {
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
#else
BStore::BStore(const char *aStoreName) : mStoreName(strdup(aStoreName)){
  mInitialized = ETrue;

  char *homeDir = getenv("HOME");

  char targetDir[4096];
  strcpy(targetDir, homeDir);
  strcat(targetDir, "/.modus");

  DIR* dir = opendir(targetDir);
  if (dir) {
    closedir(dir);
  }
  else {
    mkdir(targetDir, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH);
  }
}

BStore::~BStore() {
  FreeMem((TAny*)mStoreName);
}

TBool BStore::Get(const char *aKey, void *aValue, TUint32 aSize) {
  char name[4096];
  char *homeDir = getenv("HOME");

  strcpy(name, homeDir);
  strcat(name, "/.modus/");
  strcat(name, mStoreName);
  strcat(name, ".");
  strcat(name, aKey);
  strcat(name, ".store");

  FILE *fp = fopen(name, "r");
  if (!fp) {
    return EFalse;
  }
  if (fread(aValue, 1, aSize, fp) != aSize) {
    fclose(fp);
    return EFalse;
  }
  fclose(fp);
  return ETrue;
}

TBool BStore::Set(const char *aKey, void *aValue, TUint32 aSize){
  char name[4096];
  char *homeDir = getenv("HOME");

  strcpy(name, homeDir);
  strcat(name, "/.modus/");
  strcat(name, mStoreName);
  strcat(name, ".");
  strcat(name, aKey);
  strcat(name, ".store");

  FILE *fp = fopen(name, "w");
  if (!fp || fwrite(aValue, aSize, 1, fp) != aSize) {
    fclose(fp);
    return EFalse;
  }
  fclose(fp);
  return ETrue;
}


#endif

