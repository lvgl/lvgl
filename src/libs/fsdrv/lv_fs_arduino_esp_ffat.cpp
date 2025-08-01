#include "../../../lvgl.h"
#include "lv_fs_arduino_esp_fs.h"

#if LV_USE_FS_ARDUINO_ESP_FFAT

#include "FFat.h"

#if !LV_FS_IS_VALID_LETTER(LV_FS_ARDUINO_ESP_FFAT_LETTER)
    #error "Invalid drive letter"
#endif

// &(LV_GLOBAL_DEFAULT()->arduino_esp_fs_drv

/**
 * Register a driver for the LittleFS File System interface
 */
extern "C" void lv_fs_arduino_esp_ffat_init(void)
{
    lv_fs_arduino_esp_fs_init((esp_fs_init_t[]){{
        .letter = LV_FS_ARDUINO_ESP_FFAT_LETTER,
        .drv = &(LV_GLOBAL_DEFAULT()->arduino_esp_ffat_drv),
        .init = []() -> FS* {
            FFat.begin();
            return &FFat;
        }
    }});
}

#else /*LV_USE_FS_ARDUINO_ESP_FFAT == 0*/

#if defined(LV_FS_ARDUINO_ESP_FFAT_LETTER) && LV_FS_ARDUINO_ESP_FFAT_LETTER != '\0'
    #warning "LV_USE_FS_ARDUINO_ESP_FFAT is not enabled but LV_FS_ARDUINO_ESP_FFAT_LETTER is set"
#endif

#endif /*LV_USE_FS_ARDUINO_ESP_FFAT*/
