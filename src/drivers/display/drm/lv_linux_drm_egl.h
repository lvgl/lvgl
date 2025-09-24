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
typedef struct _lv_drm_use_egl_texture_t lv_drm_use_egl_texture_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an lv_drm_egl output with no textures and initialize OpenGL
 * @param hor_res            width in pixels of the window
 * @param ver_res            height in pixels of the window
 * @param use_mouse_indev    send pointer indev input to LVGL display textures
 * @return                   the new MK_DRM output handle
 */
lv_drm_egl_t * lv_drm_egl_create();

/**
 * Create an lv_drm_egl output with no textures and initialize OpenGL
 * @param placeholder_display 
 * @param use_mouse_indev    send pointer indev input to LVGL display textures
 * @param h_flip             Should the window contents be horizontally mirrored?
 * @param v_flip             Should the window contents be vertically mirrored?
 * @return                   the new MK_DRM output handle
 */
lv_drm_egl_t * lv_drm_egl_create_ex(lv_display_t * placeholder_display, bool use_mouse_indev, bool h_flip, bool v_flip);

lv_display_t * lv_drm_egl_get_display(lv_drm_egl_t * window, lv_display_t * placeholder_display );

/**
 * Delete an lv_drm_egl output. If it is the last one, the process will exit
 * @param window    MK_DRM output to delete
 */
void lv_drm_egl_delete(lv_drm_egl_t * window);

/**
 * Set the horizontal / vertical flipping of an lv_drm_egl output
 * @param window    MK_DRM output to configure
 * @param h_flip    Should the window contents be horizontally mirrored?
 * @param v_flip    Should the window contents be vertically mirrored?
 */
void lv_drm_egl_set_flip(lv_drm_egl_t * window, bool h_flip, bool v_flip);

/**
 * Add a texture to the MK_DRM output. It can be an LVGL display texture, or any OpenGL texture
 * @param window        MK_DRM output
 * @param texture_id    OpenGL texture ID
 * @param w             width in pixels of the texture
 * @param h             height in pixels of the texture
 * @return              the new texture handle
 */
lv_drm_use_egl_texture_t * lv_drm_egl_add_texture(lv_drm_egl_t * window,
                                                  unsigned int texture_id,
                                                  int32_t w,
                                                  int32_t h);

/**
 * Remove a texture from its MK_DRM output and delete it
 * @param texture    handle of an lv_drm_egl output texture
 */
void lv_drm_use_egl_texture_remove(lv_drm_use_egl_texture_t * texture);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_MK_DRM_WINDOW_H */
