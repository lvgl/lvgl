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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if LV_USE_FS_FATFS != '\0'
	void lv_fs_fatfs_init(void);
#endif

#if LV_USE_FS_STDIO != '\0'
	void lv_fs_stdio_init(void);
#endif

#if LV_USE_FS_POSIX != '\0'
	void lv_fs_posix_init(void);
#endif

#if LV_USE_FS_WIN32 != '\0'
	void lv_fs_win32_init(void);
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_FSDRV_H*/

