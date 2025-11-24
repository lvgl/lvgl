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
#include "lfs.h"
struct lfs;
/**
 * Set the default LittleFS handler to be used by LVGL
 * @param  lfs     pointer to an initialized LittleFS filesystem structure
 */
void lv_littlefs_set_handler(struct lfs * lfs);

/**
 * Initialize LittleFS file system driver
 */
void lv_fs_littlefs_init(void);

/**
 * Register a LittleFS drive with LVGL
 * @param  lfs     pointer to an initialized LittleFS filesystem structure
 * @param  letter  driver letter to register (e.g. 'A')
 * @return         LV_FS_RES_OK: success, LV_FS_RES_INV_PARAM: lfs is NULL or letter not in range A-Z,
 *                 LV_FS_RES_DRIVE_LETTER_ALREADY_USED: A drive with this letter is already registered
 */
lv_fs_res_t lv_fs_littlefs_register_drive(lfs_t * lfs, char letter);
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

#if LV_USE_FS_FROGFS
void lv_fs_frogfs_init(void);
void lv_fs_frogfs_deinit(void);

/**
 * Mount a frogfs blob at the path prefix. If there is a file "foo.txt"
 * in the blob and the blob is registered with `path_prefix` as "my_blob",
 * it can be opened later at path "my_blob/foo.txt".
 * @param blob         a frogfs blob/image from mkfrogfs.py
 * @param path_prefix  a prefix that will be used to refer to this blob when accessing it.
 * @return             LV_RESULT_OK or LV_RESULT_INVALID if there was an issue with the blob
 */
lv_result_t lv_fs_frogfs_register_blob(const void * blob, const char * path_prefix);

/**
 * Unmount a frogfs blob that was previously mounted by `lv_fs_frogfs_register_blob`.
 * All files and dirs should be closed before calling this.
 * @param path_prefix  the path prefix that the blob was registered with
 */
void lv_fs_frogfs_unregister_blob(const char * path_prefix);

#endif /*LV_USE_FS_FROGFS*/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_FSDRV_H*/
