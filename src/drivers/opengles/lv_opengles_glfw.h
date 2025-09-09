/**
 * @file lv_opengles_glfw.h
 *
 */

#ifndef LV_OPENGLES_GLFW_H
#define LV_OPENGLES_GLFW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_conf_internal.h"
#if LV_USE_GLFW

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

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a GLFW OpenGL window with no textures and initialize OpenGL
 * @param hor_res            width in pixels of the window
 * @param ver_res            height in pixels of the window
 * @param use_mouse_indev    send pointer indev input to LVGL display textures
 * @return                   the new GLFW OpenGL window handle
 */
lv_opengles_window_t * lv_opengles_glfw_window_create(int32_t hor_res, int32_t ver_res, bool use_mouse_indev);

/**
 * Create a GLFW window with no textures and initialize OpenGL
 * @param hor_res            width in pixels of the window
 * @param ver_res            height in pixels of the window
 * @param use_mouse_indev    send pointer indev input to LVGL display textures
 * @param h_flip             Should the window contents be horizontally mirrored?
 * @param v_flip             Should the window contents be vertically mirrored?
 * @param title              The window title
 * @return                   the new GLFW window handle
 */
lv_opengles_window_t * lv_opengles_glfw_window_create_ex(int32_t hor_res, int32_t ver_res, bool use_mouse_indev,
                                                         bool h_flip, bool v_flip,  const char * title);

/**
 * Set the window's title text
 * @param window     GLFW window to configure
 * @param new_title  The new title text
 */
void lv_opengles_glfw_window_set_title(lv_opengles_window_t * window, const char * new_title);

/**
 * Set the horizontal / vertical flipping of a GLFW window
 * @param window    GLFW window to configure
 * @param h_flip    Should the window contents be horizontally mirrored?
 * @param v_flip    Should the window contents be vertically mirrored?
 */
void lv_opengles_glfw_window_set_flip(lv_opengles_window_t * window, bool h_flip, bool v_flip);

/**
 * Get the GLFW window handle for a GLFW lv_opengles_window_t
 * @param window        GLFW window to return the handle of
 * @return              the GLFW window handle
 */
void * lv_opengles_glfw_window_get_glfw_window(lv_opengles_window_t * window);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GLFW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGLES_GLFW_H*/
