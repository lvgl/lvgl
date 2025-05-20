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

/**
 * Create a GLFW window with no textures and initialize OpenGL
 * @param hor_res            width in pixels of the window
 * @param ver_res            height in pixels of the window
 * @param use_mouse_indev    send pointer indev input to LVGL display textures
 * @return                   the new GLFW window handle
 */
lv_glfw_window_t * lv_glfw_window_create(int32_t hor_res, int32_t ver_res, bool use_mouse_indev);

/**
 * Delete a GLFW window. If it is the last one, the process will exit
 * @param window    GLFW window to delete
 */
void lv_glfw_window_delete(lv_glfw_window_t * window);

/**
 * Get the GLFW window handle for an lv_glfw_window
 * @param window        GLFW window to return the handle of
 * @return              the GLFW window handle
 */
void * lv_glfw_window_get_glfw_window(lv_glfw_window_t * window);

/**
 * Add a texture to the GLFW window. It can be an LVGL display texture, or any OpenGL texture
 * @param window        GLFW window
 * @param texture_id    OpenGL texture ID
 * @param w             width in pixels of the texture
 * @param h             height in pixels of the texture
 * @return              the new texture handle
 */
lv_glfw_texture_t * lv_glfw_window_add_texture(lv_glfw_window_t * window, unsigned int texture_id, int32_t w,
                                               int32_t h);

/**
 * Remove a texture from its GLFW window and delete it
 * @param texture    handle of a GLFW window texture
 */
void lv_glfw_texture_remove(lv_glfw_texture_t * texture);

/**
 * Set the x position of a texture within its GLFW window
 * @param texture    handle of a GLFW window texture
 * @param x          new x position of the texture
 */
void lv_glfw_texture_set_x(lv_glfw_texture_t * texture, int32_t x);

/**
 * Set the y position of a texture within its GLFW window
 * @param texture    handle of a GLFW window texture
 * @param y          new y position of the texture
 */
void lv_glfw_texture_set_y(lv_glfw_texture_t * texture, int32_t y);

/**
 * Set the opacity of a texture in a GLFW window
 * @param texture    handle of a GLFW window texture
 * @param opa        new opacity of the texture
 */
void lv_glfw_texture_set_opa(lv_glfw_texture_t * texture, lv_opa_t opa);

/**
 * Get the mouse indev associated with a texture in a GLFW window, if it exists
 * @param texture    handle of a GLFW window texture
 * @return           the indev or `NULL`
 * @note             there will only be an indev if the texture is based on an
 *                   LVGL display texture and the window was created with
 *                   `use_mouse_indev` as `true`
 */
lv_indev_t * lv_glfw_texture_get_mouse_indev(lv_glfw_texture_t * texture);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_OPENGLES */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_GLFW_WINDOW_H */
