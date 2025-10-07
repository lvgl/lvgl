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
#include "../../lv_conf_internal.h"

#if LV_USE_OPENGLES

#include "lv_opengles_texture.h"

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

lv_result_t lv_opengles_texture_create_draw_buffers(lv_opengles_texture_t * texture, lv_display_t * display);
void lv_opengles_texture_reshape(lv_display_t * disp, int32_t width, int32_t height);
void lv_opengles_texture_deinit(lv_opengles_texture_t * texture);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OPENGLES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_OPENGLES_TEXTURE_PRIVATE_H */
