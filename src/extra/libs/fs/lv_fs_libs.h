/**
 * @file lv_fs_libs.h
 *
 */

#ifndef LV_FS_LIBS_H
#define LV_FS_LIBS_H

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
	lv_fs_fatfs_init();
#endif

#if LV_USE_FS_STDIO != '\0'
	lv_fs_stdio_init();
#endif

#if LV_USE_FS_POSIX != '\0'
    lv_fs_posix_init();
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_FS_LIBS_H*/

