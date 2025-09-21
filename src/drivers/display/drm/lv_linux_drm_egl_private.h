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
#include "../../../indev/lv_indev.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_drm_egl_t {
    lv_egl_adapter_interface_t * egl_adapter_interface;
    lv_display_t * display_texture;
    lv_drm_use_egl_texture_t * window_texture;
    int32_t hor_res;
    int32_t ver_res;
    bool h_flip;
    bool v_flip;
    lv_ll_t textures;
    lv_point_t mouse_last_point;
    lv_indev_state_t mouse_last_state;
    uint8_t closing : 1;
};

struct _lv_drm_use_egl_texture_t {
    lv_drm_egl_t * window;
    unsigned int texture_id;
    lv_area_t area;
    lv_opa_t opa;
    lv_indev_t * indev;
    lv_point_t indev_last_point;
    lv_indev_state_t indev_last_state;
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
