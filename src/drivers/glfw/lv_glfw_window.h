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

lv_glfw_window_t * lv_glfw_window_create(int32_t hor_res, int32_t ver_res, bool use_mouse_indev);
void lv_glfw_window_delete(lv_glfw_window_t * window);
void lv_glfw_window_make_context_current(lv_glfw_window_t * window);
lv_glfw_texture_t * lv_glfw_window_add_texture(lv_glfw_window_t * window, unsigned int texture_id, int32_t w,
                                               int32_t h);
void lv_glfw_texture_remove(lv_glfw_texture_t * texture);
void lv_glfw_texture_set_x(lv_glfw_texture_t * texture, int32_t x);
void lv_glfw_texture_set_y(lv_glfw_texture_t * texture, int32_t y);
void lv_glfw_texture_set_opa(lv_glfw_texture_t * texture, lv_opa_t opa);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_OPENGLES */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_GLFW_WINDOW_H */
