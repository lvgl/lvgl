/**
 * @file lv_fullscreen_drm_use_egl.h
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

#include "../../../../lv_conf_internal.h"
#if LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL

#include "../../../../misc/lv_types.h"
#include "../../../../display/lv_display.h"

#include "../../../opengles/egl_adapter/common/opengl_headers.h"

//#include "../egl_adapter_outmod/lv_egl_adapter_outmod_drm.h"

/*********************
 *      DEFINES
 *********************/

#define __LV_OUTMOD_CLASS_T lv_egl_adapter_outmod_drm_t
#define __LV_OUTMOD_CLASS_INCLUDE_T "../../display/drm/egl_adapter_outmod/lv_egl_adapter_outmod_drm.h"
#include "../../../opengles/egl_adapter/lv_egl_adapter_interface.h"

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_fullscreen_drm_use_egl_t lv_fullscreen_drm_use_egl_t;
typedef struct _lv_drm_use_egl_texture_t lv_drm_use_egl_texture_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a MK_DRM output with no textures and initialize OpenGL
 * @param hor_res            width in pixels of the window
 * @param ver_res            height in pixels of the window
 * @param use_mouse_indev    send pointer indev input to LVGL display textures
 * @return                   the new MK_DRM output handle
 */
lv_fullscreen_drm_use_egl_t * lv_fullscreen_drm_use_egl_create();

/**
 * Create a MK_DRM output with no textures and initialize OpenGL
 * @param hor_res            width in pixels of the window
 * @param ver_res            height in pixels of the window
 * @param use_mouse_indev    send pointer indev input to LVGL display textures
 * @param h_flip             Should the window contents be horizontally mirrored?
 * @param v_flip             Should the window contents be vertically mirrored?
 * @return                   the new MK_DRM output handle
 */
lv_fullscreen_drm_use_egl_t * lv_fullscreen_drm_use_egl_create_ex(bool use_mouse_indev, bool h_flip, bool v_flip);

lv_display_t * lv_fullscreen_drm_use_egl_get_display(lv_fullscreen_drm_use_egl_t * window);

/**
 * Delete a MK_DRM output. If it is the last one, the process will exit
 * @param window    MK_DRM output to delete
 */
void lv_fullscreen_drm_use_egl_delete(lv_fullscreen_drm_use_egl_t * window);

/**
 * Set the horizontal / vertical flipping of a MK_DRM output
 * @param window    MK_DRM output to configure
 * @param h_flip    Should the window contents be horizontally mirrored?
 * @param v_flip    Should the window contents be vertically mirrored?
 */
void lv_fullscreen_drm_use_egl_set_flip(lv_fullscreen_drm_use_egl_t * window, bool h_flip, bool v_flip);

/**
 * Get the MK_DRM output handle for an lv_fullscreen_drm_use_egl
 * @param window        MK_DRM output to return the handle of
 * @return              the MK_DRM output handle
 */
void * lv_fullscreen_drm_use_egl_get_drm_window(lv_fullscreen_drm_use_egl_t * window);

/**
 * Add a texture to the MK_DRM output. It can be an LVGL display texture, or any OpenGL texture
 * @param window        MK_DRM output
 * @param texture_id    OpenGL texture ID
 * @param w             width in pixels of the texture
 * @param h             height in pixels of the texture
 * @return              the new texture handle
 */
lv_drm_use_egl_texture_t * lv_fullscreen_drm_use_egl_add_texture(lv_fullscreen_drm_use_egl_t * window,
                                                                 unsigned int texture_id,
                                                                 int32_t w,
                                                                 int32_t h);

/**
 * Remove a texture from its MK_DRM output and delete it
 * @param texture    handle of a MK_DRM output texture
 */
void lv_drm_use_egl_texture_remove(lv_drm_use_egl_texture_t * texture);

/**
 * Set the x position of a texture within its MK_DRM output
 * @param texture    handle of a MK_DRM output texture
 * @param x          new x position of the texture
 */
void lv_drm_use_egl_texture_set_x(lv_drm_use_egl_texture_t * texture, int32_t x);

/**
 * Set the y position of a texture within its MK_DRM output
 * @param texture    handle of a MK_DRM output texture
 * @param y          new y position of the texture
 */
void lv_drm_use_egl_texture_set_y(lv_drm_use_egl_texture_t * texture, int32_t y);

/**
 * Set the opacity of a texture in a MK_DRM output
 * @param texture    handle of a MK_DRM output texture
 * @param opa        new opacity of the texture
 */
void lv_drm_use_egl_texture_set_opa(lv_drm_use_egl_texture_t * texture, lv_opa_t opa);

/**
 * Get the mouse indev associated with a texture in a MK_DRM output, if it exists
 * @param texture    handle of a MK_DRM output texture
 * @return           the indev or `NULL`
 * @note             there will only be an indev if the texture is based on an
 *                   LVGL display texture and the window was created with
 *                   `use_mouse_indev` as `true`
 */
lv_indev_t * lv_drm_use_egl_texture_get_mouse_indev(lv_drm_use_egl_texture_t * texture);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_MK_DRM_WINDOW_H */
