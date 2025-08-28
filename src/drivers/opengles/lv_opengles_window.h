/**
 * @file lv_opengles_window.h
 *
 */

#ifndef LV_OPENGLES_WINDOW_H
#define LV_OPENGLES_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_OPENGLES

#include "../../misc/lv_types.h"

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
 * Delete an OpenGL window. If it is the last one, the process will exit
 * @param window    OpenGL window to delete
 */
void lv_opengles_window_delete(lv_opengles_window_t * window);

/**
 * Add a texture to the OpenGL window. It can be an LVGL display texture, or any OpenGL texture
 * @param window        OpenGL window
 * @param texture_id    OpenGL texture ID
 * @param w             width in pixels of the texture
 * @param h             height in pixels of the texture
 * @return              the new texture handle
 */
lv_opengles_window_texture_t * lv_opengles_window_add_texture(lv_opengles_window_t * window, unsigned int texture_id,
                                                              int32_t w, int32_t h);

lv_display_t * lv_opengles_window_display_create(lv_opengles_window_t * window, int32_t w, int32_t h);

lv_opengles_window_texture_t * lv_opengles_window_display_get_window_texture(lv_display_t * window_display);

/**
 * Remove a texture from its OpenGL window and delete it
 * @param texture    handle of an OpenGL window texture
 */
void lv_opengles_window_texture_remove(lv_opengles_window_texture_t * texture);

/**
 * Set the x position of a texture within its OpenGL window
 * @param texture    handle of an OpenGL window texture
 * @param x          new x position of the texture
 */
void lv_opengles_window_texture_set_x(lv_opengles_window_texture_t * texture, int32_t x);

/**
 * Set the y position of a texture within its OpenGL window
 * @param texture    handle of an OpenGL window texture
 * @param y          new y position of the texture
 */
void lv_opengles_window_texture_set_y(lv_opengles_window_texture_t * texture, int32_t y);

/**
 * Set the opacity of a texture in an OpenGL window
 * @param texture    handle of an OpenGL window texture
 * @param opa        new opacity of the texture
 */
void lv_opengles_window_texture_set_opa(lv_opengles_window_texture_t * texture, lv_opa_t opa);

/**
 * Get the mouse indev associated with a texture in an OpenGL window, if it exists
 * @param texture    handle of an OpenGL window texture
 * @return           the indev or `NULL`
 * @note             there will only be an indev if the texture is based on an
 *                   LVGL display texture and the window was created with
 *                   `use_mouse_indev` as `true`
 */
lv_indev_t * lv_opengles_window_texture_get_mouse_indev(lv_opengles_window_texture_t * texture);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_OPENGLES */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_OPENGLES_WINDOW_H */
