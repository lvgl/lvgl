/**
 * @file lv_draw_sdl_rect.h
 *
 */

#ifndef LV_DRAW_SDL_RECT_H
#define LV_DRAW_SDL_RECT_H


#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include LV_GPU_SDL_INCLUDE_PATH

#include "../lv_draw.h"

#include "lv_draw_sdl_texture_cache.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct lv_draw_sdl_rect_header_t {
    lv_img_header_t base;
    SDL_Rect rect;
} lv_draw_sdl_rect_header_t;

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
SDL_Texture * lv_draw_sdl_rect_obtain_bg_frag(lv_draw_sdl_ctx_t * ctx, lv_coord_t radius);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_SDL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SDL_RECT_H*/
