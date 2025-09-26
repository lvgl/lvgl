/**
 * @file lv_linux_drm_private.h
 *
 */

#ifndef LV_LINUX_DRM_PRIVATE_H
#define LV_LINUX_DRM_PRIVATE_H

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

char * lv_linux_drm_find_device_path(void);
int lv_linux_drm_open_by_module_name(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LINUX_DRM_PRIVATE_H*/
