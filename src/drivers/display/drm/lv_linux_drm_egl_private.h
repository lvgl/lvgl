/**
 * @file lv_linux_drm_egl_private.h
 *
 */

#ifndef LV_LINUX_DRM_EGL_PRIVATE_H
#define LV_LINUX_DRM_EGL_PRIVATE_H


#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL

#include <xf86drmMode.h>
#include "../../opengles/lv_opengles_texture_private.h"
#include "../../opengles/lv_opengles_egl.h"
#include "../../opengles/lv_opengles_egl_private.h"
#include "lv_linux_drm.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_opengles_texture_t texture;
    lv_display_t * display;
    lv_opengles_egl_t * egl_ctx;
    lv_egl_interface_t egl_interface;

    drmModeRes * drm_resources;
    drmModeConnector * drm_connector;
    drmModeEncoder * drm_encoder;
    drmModeCrtc * drm_crtc;
    drmModeModeInfo * drm_mode;

    struct gbm_device * gbm_dev;
    struct gbm_surface * gbm_surface;
    struct gbm_bo * gbm_bo_pending;
    struct gbm_bo * gbm_bo_flipped;
    struct gbm_bo * gbm_bo_presented;

    lv_linux_drm_select_mode_cb_t mode_select_cb;
    int fd;
    bool crtc_isset;
} lv_drm_ctx_t;



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


#endif /*LV_LINUX_DRM_EGL_PRIVATE_H*/
