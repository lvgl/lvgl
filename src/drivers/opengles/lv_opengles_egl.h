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


#if LV_USE_EGL

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
 * @param device                a `EGLNativeDisplayType`
 * @param pre                   called before rendering
 * @param post1                 called after rendering, before swapping
 * @param post2                 called after swapping
 * @return                      the new EGL OpenGL window handle
 */
lv_opengles_window_t * lv_opengles_egl_window_create(int32_t hor_res, int32_t ver_res, void * native_window_handle,
                                                     void * device,
                                                     lv_opengles_egl_window_cb_t pre,
                                                     lv_opengles_egl_window_cb_t post1,
                                                     lv_opengles_egl_window_cb_t post2);

void * lv_opengles_egl_window_get_surface(lv_opengles_window_t * window);
void * lv_opengles_egl_window_get_display(lv_opengles_window_t * window);
void lv_opengles_egl_window_set_user_data(lv_opengles_window_t * window, void * user_data);
void * lv_opengles_egl_window_get_user_data(lv_opengles_window_t * window);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_EGL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGLES_EGL_H*/
