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

lv_display_t * lv_opengles_texture_create(int32_t w, int32_t h);
bool lv_opengles_texture_get_texture_id(lv_display_t * disp, unsigned int * texture_id_dst);
lv_display_t * lv_opengles_texture_get_from_texture_id(unsigned int texture_id);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_OPENGLES */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGLES_TEXTURE_H*/
