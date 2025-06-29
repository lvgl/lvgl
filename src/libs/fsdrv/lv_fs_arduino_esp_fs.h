#ifndef lv_fs_arduino_esp_fs_h
#define lv_fs_arduino_esp_fs_h

#include "../../core/lv_global.h"

#if LV_USE_FS_ARDUINO_ESP_FS

#include <FS.h>

typedef FS *(*init_fs_cb_t)(void);

void lv_fs_arduino_esp_fs_init(const init_fs_cb_t init_cb);

#endif // LV_USE_FS_ARDUINO_ESP_FS

#endif