#include "../../../lvgl.h"
#include "lv_fs_arduino_esp_fs.h"

#if LV_USE_FS_ARDUINO_ESP_LITTLEFS

#include <LittleFS.h>

#if !LV_FS_IS_VALID_LETTER(LV_FS_ARDUINO_ESP_LITTLEFS_LETTER)
    #error "Invalid drive letter"
#endif

/**
 * Register a driver for the LittleFS File System interface
 */
extern "C" void lv_fs_arduino_esp_littlefs_init(void)
{
    lv_fs_arduino_esp_fs_init((esp_fs_init_t[]){{
        .letter = LV_FS_ARDUINO_ESP_LITTLEFS_LETTER,
        .drv = &(LV_GLOBAL_DEFAULT()->arduino_esp_littlefs_drv),
        .init = []() -> FS* {
            LittleFS.begin();
            return &LittleFS;
        }
    }});
}

#else /*LV_USE_FS_ARDUINO_ESP_LITTLEFS == 0*/

#if defined(LV_FS_ARDUINO_ESP_LITTLEFS_LETTER) && LV_FS_ARDUINO_ESP_LITTLEFS_LETTER != '\0'
    #warning "LV_USE_FS_ARDUINO_ESP_LITTLEFS is not enabled but LV_FS_ARDUINO_ESP_LITTLEFS_LETTER is set"
#endif

#endif /*LV_USE_FS_ARDUINO_ESP_LITTLEFS*/
