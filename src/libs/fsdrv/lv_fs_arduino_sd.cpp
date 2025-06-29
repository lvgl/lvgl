#include "../../../lvgl.h"
#include "lv_fs_arduino_esp_fs.h"

#if LV_USE_FS_ARDUINO_SD && LV_USE_FS_ARDUINO_ESP_FS

#include <SD.h>

#if !LV_FS_IS_VALID_LETTER(LV_FS_ARDUINO_SD_LETTER)
    #error "Invalid drive letter"
#endif

/**
 * Register a driver for the SD File System interface
 */
extern "C" void lv_fs_arduino_sd_init(void)
{
    lv_fs_arduino_esp_fs_init([]() -> FS* { return &SD });
}

#else /*LV_USE_FS_ARDUINO_SD == 0*/

#if defined(LV_FS_ARDUINO_SD_LETTER) && LV_FS_ARDUINO_SD_LETTER != '\0'
    #warning "LV_USE_FS_ARDUINO_SD is not enabled but LV_FS_ARDUINO_SD_LETTER is set"
#endif

#endif /*LV_USE_FS_ARDUINO_SD*/
