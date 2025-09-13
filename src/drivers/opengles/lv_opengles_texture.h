/**
 * @file lv_opengles_texture.h
 *
 */

#ifndef LV_OPENGLES_TEXTURE_H
#define LV_OPENGLES_TEXTURE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_OPENGLES

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
 * Create a display that flushes to an OpenGL texture
 * If you already have a texture and want to bind it to the display,
 *    see `lv_opengles_texture_create_from_texture_id`
 * @param w    width in pixels of the texture
 * @param h    height in pixels of the texture
 * @return     the new display or NULL on failure
 */
lv_display_t * lv_opengles_texture_create(int32_t w, int32_t h);

/**
 * Create a display that flushes to the provided OpenGL texture
 * If you don't have a texture to bind it to the display,
 *    see `lv_opengles_texture_create`
 * @param w         width in pixels of the texture
 * @param h         height in pixels of the texture
 * @param texture_id    the texture LVGL will render to
 * @return     the new display or NULL on failure
 */
lv_display_t * lv_opengles_texture_create_from_texture_id(int32_t w, int32_t h, unsigned int texture_id);

/**
 * Get the OpenGL texture ID of the display
 * @param disp    display
 * @return        texture ID
 */
unsigned int lv_opengles_texture_get_texture_id(lv_display_t * disp);

/**
 * Get the display of an OpenGL texture if it is associated with one
 * @param texture_id   OpenGL texture ID
 * @return             display or `NULL` if there no display with that texture ID
 */
lv_display_t * lv_opengles_texture_get_from_texture_id(unsigned int texture_id);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_OPENGLES */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGLES_TEXTURE_H*/
