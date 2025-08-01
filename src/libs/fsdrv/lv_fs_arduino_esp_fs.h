#ifndef lv_fs_arduino_esp_fs_h
#define lv_fs_arduino_esp_fs_h

#include "../../core/lv_global.h"

/**
 * The implementation of the file system drivers in Arduino has a single FS and File interface, 
 * so it was decided to wrap this into a single thread-safe model.
 */

// If defined one of FS_ARDUINO_ESP_ file system
#define LV_USE_FS_ARDUINO_ESP_FS (((LV_USE_FS_ARDUINO_ESP_FFAT != 0) + (LV_USE_FS_ARDUINO_ESP_LITTLEFS != 0) + (LV_USE_FS_ARDUINO_SD != 0) == 1))

#if LV_USE_FS_ARDUINO_ESP_FS

#include <FS.h>

typedef FS *(*init_fs_cb_t)(void);

typedef struct {
    char letter;
    lv_fs_drv_t* drv;
    init_fs_cb_t init;
} esp_fs_init_t;

void lv_fs_arduino_esp_fs_init(esp_fs_init_t* esp_fs_init);

#endif // LV_USE_FS_ARDUINO_ESP_FS

#endif