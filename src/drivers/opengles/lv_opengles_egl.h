/**
 * @file lv_opengles_egl.h
 *
 */

#ifndef LV_OPENGLES_EGL_H
#define LV_OPENGLES_EGL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_conf_internal.h"
#if LV_USE_OPENGLES && LV_USE_OPENGLES_API == LV_OPENGLES_API_EGL

#include "../../misc/lv_types.h"

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef void (*lv_opengles_egl_window_cb_t)(lv_opengles_window_t * window);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an EGL OpenGL window with no textures. OpenGL should be initialized beforehand.
 * @param hor_res               width in pixels of the window
 * @param ver_res               height in pixels of the window
 * @param native_window_handle  a handle to a native window for the platform
 * @return                      the new EGL OpenGL window handle
 */
lv_opengles_window_t * lv_opengles_egl_window_create(int32_t hor_res, int32_t ver_res, void * native_window_handle,
                                                     lv_opengles_egl_window_cb_t pre,
                                                     lv_opengles_egl_window_cb_t post1,
                                                     lv_opengles_egl_window_cb_t post2);

void * lv_opengles_egl_window_get_surface(lv_opengles_window_t * window);
void * lv_opengles_egl_window_get_display(lv_opengles_window_t * window);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OPENGLES && LV_USE_OPENGLES_API == LV_OPENGLES_API_EGL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGLES_EGL_H*/
