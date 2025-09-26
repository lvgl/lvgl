/**
 * @file lv_drm_egl_private.h
 *
 */

#ifndef LV_MK_DRM_WINDOW_PRIVATE_H
#define LV_MK_DRM_WINDOW_PRIVATE_H


#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_linux_drm_egl.h"
#if LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL

#include "../../../misc/lv_area.h"
#include "../../../display/lv_display.h"
#include <xf86drmMode.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_drm_egl_t {
    lv_opengles_texture_t texture;
    lv_egl_adapter_interface_t * egl_interface;
    lv_display_t * display;
    bool h_flip;
    bool v_flip;
};

struct lv_egl_adapter_outmod_drm {
    lv_egl_adapter_output_core_t * core;
    int fd;
    drmModeRes * drm_resources;
    drmModeConnector * drm_connector;
    drmModeEncoder * drm_encoder;
    drmModeCrtcPtr drm_crtc;
    drmModeModeInfo * drm_mode;
    struct gbm_device * gbm_dev;
    struct gbm_surface * gbm_surface;
    struct gbm_bo * gbm_bo_pending;
    struct gbm_bo * gbm_bo_flipped;
    struct gbm_bo * gbm_bo_presented;
    bool crtc_isset;
    bool async_flip;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif


#endif /*LV_MK_DRM_WINDOW_PRIVATE_H*/
