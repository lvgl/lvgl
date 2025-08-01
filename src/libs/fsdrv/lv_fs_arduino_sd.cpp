#include "../../../lvgl.h"
#include "lv_fs_arduino_esp_fs.h"

#if LV_USE_FS_ARDUINO_SD

#include <SD.h>

#if !LV_FS_IS_VALID_LETTER(LV_FS_ARDUINO_SD_LETTER)
    #error "Invalid drive letter"
#endif

/**
 * Register a driver for the SD File System interface
 */
extern "C" void lv_fs_arduino_sd_init(void)
{
    lv_fs_arduino_esp_fs_init((esp_fs_init_t[]) {
        {
            .letter = LV_FS_ARDUINO_SD_LETTER,
            .drv = &(LV_GLOBAL_DEFAULT()->arduino_esp_sd_drv),
            .init = []() -> FS* { return &SD }
        }
    });
}

#else /*LV_USE_FS_ARDUINO_SD == 0*/

#if defined(LV_FS_ARDUINO_SD_LETTER) && LV_FS_ARDUINO_SD_LETTER != '\0'
    #warning "LV_USE_FS_ARDUINO_SD is not enabled but LV_FS_ARDUINO_SD_LETTER is set"
#endif

#endif /*LV_USE_FS_ARDUINO_SD*/
