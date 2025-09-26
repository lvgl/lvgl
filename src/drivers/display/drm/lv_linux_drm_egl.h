/**
 * @file lv_drm_egl.h
 *
 */

#ifndef LV_MK_DRM_WINDOW_H
#define LV_MK_DRM_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"
#if LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL

#include "../../../misc/lv_types.h"
#include "../../../display/lv_display.h"

#include "../../opengles/egl_adapter/common/opengl_headers.h"

//#include "../egl_adapter_outmod/lv_egl_adapter_outmod_drm.h"

/*********************
 *      DEFINES
 *********************/

#define __LV_OUTMOD_CLASS_T lv_egl_adapter_outmod_drm_t
#define __LV_OUTMOD_CLASS_INCLUDE_T "../display/drm/egl_adapter_outmod/lv_egl_adapter_outmod_drm.h"
#include "../../opengles/egl_adapter/lv_egl_adapter_interface.h"

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_drm_egl_t lv_drm_egl_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Set the horizontal / vertical flipping of an lv_drm_egl output
 * @param window    MK_DRM output to configure
 * @param h_flip    Should the window contents be horizontally mirrored?
 * @param v_flip    Should the window contents be vertically mirrored?
 */
void lv_drm_egl_set_flip(lv_drm_egl_t * window, bool h_flip, bool v_flip);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_MK_DRM_WINDOW_H */
