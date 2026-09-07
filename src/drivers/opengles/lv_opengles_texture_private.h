/**
 * @file lv_opengles_texture_private.h
 *
 */


#ifndef LV_OPENGLES_TEXTURE_PRIVATE_H
#define LV_OPENGLES_TEXTURE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl_public.h"

#if LV_USE_OPENGLES

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    unsigned int texture_id;
#if !LV_USE_DRAW_OPENGLES
    uint8_t * fb1;
#endif /*!LV_USE_DRAW_OPENGLES*/
    bool is_texture_owner;
} lv_opengles_texture_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_result_t lv_opengles_texture_reshape(lv_opengles_texture_t * texture, lv_display_t * display,
                                        int32_t width, int32_t height);
void lv_opengles_texture_deinit(lv_opengles_texture_t * texture);

/**
 * Render the display content to the current OpenGL framebuffer
 * Handles NanoVG, OpenGL and SW rendering
 * @param texture   the texture of the display
 * @param display   the display to render
 */
void lv_opengles_texture_render_display(lv_opengles_texture_t * texture, lv_display_t * display);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OPENGLES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_OPENGLES_TEXTURE_PRIVATE_H */
