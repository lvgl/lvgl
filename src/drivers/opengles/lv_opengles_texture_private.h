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

/**********************
 *      MACROS
 **********************/

lv_result_t lv_opengles_texture_init_display(lv_display_t * disp, int32_t w, int32_t h);

#endif /*LV_USE_OPENGLES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_OPENGLES_TEXTURE_PRIVATE_H */
