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

#include "../../../lvgl_public.h"

#if LV_USE_LINUX_DRM

#include <xf86drmMode.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_linux_drm_mode_t {
    drmModeModeInfo * mode_info;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LINUX_DRM*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LINUX_DRM_PRIVATE_H*/
