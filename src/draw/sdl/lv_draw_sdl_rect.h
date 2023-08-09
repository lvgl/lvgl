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

/**
 *
 * @param ctx Drawing context
 * @param radius Corner radius of the rectangle
 * @param in_cache Whether the texture has been put in the cache
 * @return Background fragment texture
 */
SDL_Texture * lv_draw_sdl_rect_bg_frag_obtain(lv_draw_sdl_ctx_t * ctx, lv_coord_t radius, bool * in_cache);

/**
 *
 * @param ctx Drawing context
 * @param grad Gradient info
 * @param w Width of the rectangle
 * @param h Height of the rectangle
 * @param radius Corner radius of the rectangle
 * @param in_cache Whether the texture has been put in the cache
 * @return Gradient fragment texture
 */
SDL_Texture * lv_draw_sdl_rect_grad_frag_obtain(lv_draw_sdl_ctx_t * ctx, const lv_grad_dsc_t * grad, lv_coord_t w,
                                                lv_coord_t h, lv_coord_t radius, bool * in_cache);

/**
 *
 * @param ctx Drawing context
 * @param grad Gradient info
 * @param in_cache Whether the texture has been put in the cache
 * @return Gradient strip texture
 */
SDL_Texture * lv_draw_sdl_rect_grad_strip_obtain(lv_draw_sdl_ctx_t * ctx, const lv_grad_dsc_t * grad, bool * in_cache);

void lv_draw_sdl_rect_bg_frag_draw_corners(lv_draw_sdl_ctx_t * ctx, SDL_Texture * frag, lv_coord_t frag_size,
                                           const lv_area_t * coords, const lv_area_t * clip, bool full);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_SDL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SDL_RECT_H*/
