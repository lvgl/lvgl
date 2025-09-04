/**
 * @file lv_fsdrv.h
 *
 */

#ifndef LV_FSDRV_H
#define LV_FSDRV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

/*********************
 *      DEFINES
 *********************/

#define LV_FS_MAX_PATH_LEN 256

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if LV_USE_FS_FATFS
void lv_fs_fatfs_init(void);
#endif

#if LV_USE_FS_STDIO
void lv_fs_stdio_init(void);
#endif

#if LV_USE_FS_POSIX
void lv_fs_posix_init(void);
#endif

#if LV_USE_FS_WIN32
void lv_fs_win32_init(void);
#endif

#if LV_USE_FS_MEMFS
void lv_fs_memfs_init(void);
#endif

#if LV_USE_FS_LITTLEFS
struct lfs;
void lv_littlefs_set_handler(struct lfs *);
void lv_fs_littlefs_init(void);
#endif

#if LV_USE_FS_ARDUINO_ESP_LITTLEFS
void lv_fs_arduino_esp_littlefs_init(void);
#endif

#if LV_USE_FS_ARDUINO_SD
void lv_fs_arduino_sd_init(void);
#endif

#if LV_USE_FS_UEFI
void lv_fs_uefi_init(void);
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_FSDRV_H*/
