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
 * @param w    width in pixels of the texture
 * @param h    height in pixels of the texture
 * @return     the new display
 */
lv_display_t * lv_opengles_texture_create(int32_t w, int32_t h);

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
