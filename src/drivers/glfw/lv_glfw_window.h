/**
 * @file lv_glfw_window.h
 *
 */

#ifndef LV_GLFW_WINDOW_H
#define LV_GLFW_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_OPENGLES

#include "../../misc/lv_types.h"
#include "../../display/lv_display.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_display_t * lv_glfw_window_create(int32_t hor_res, int32_t ver_res);
void lv_glfw_window_make_context_current(lv_display_t * disp);
lv_glfw_texture_t * lv_glfw_texture_add(lv_display_t * disp, unsigned int texture_id, int32_t w, int32_t h);
void lv_glfw_texture_remove(lv_glfw_texture_t * texture);
void lv_glfw_texture_set_x(lv_glfw_texture_t * texture, int32_t x);
void lv_glfw_texture_set_y(lv_glfw_texture_t * texture, int32_t y);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_OPENGLES */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_GLFW_WINDOW_H */
