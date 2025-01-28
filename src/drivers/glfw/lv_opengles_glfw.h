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
#if LV_USE_OPENGLES && LV_USE_OPENGLES_API == LV_OPENGLES_API_GLFW

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
 * Get the GLFW window handle for an lv_glfw_window
 * @param window        GLFW window to return the handle of
 * @return              the GLFW window handle
 */
void * lv_opengles_glfw_window_get_glfw_window(lv_glfw_window_t * window);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OPENGLES && LV_USE_OPENGLES_API == LV_OPENGLES_API_GLFW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGLES_GLFW_H*/
