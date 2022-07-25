/**
 * @file lv_draw_sdl_img.h
 *
 */

#ifndef LV_DRAW_SDL_IMG_H
#define LV_DRAW_SDL_IMG_H


#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_SDL

#include LV_DRAW_SDL_INCLUDE_PATH

#include "../lv_draw.h"

#include "lv_draw_sdl_texture_cache.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct lv_draw_sdl_img_header_t {
    lv_img_header_t base;
    SDL_Rect rect;
} lv_draw_sdl_img_header_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/*=====================
 * Getter functions
 *====================*/

/*=====================
 * Other functions
 *====================*/
bool lv_draw_sdl_img_load_texture(lv_draw_sdl_ctx_t * ctx, lv_draw_sdl_cache_key_head_img_t * key, size_t key_size,
                                  const void * src, int32_t frame_id, SDL_Texture ** texture,
                                  lv_draw_sdl_img_header_t ** header);
/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_SDL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SDL_IMG_H*/
