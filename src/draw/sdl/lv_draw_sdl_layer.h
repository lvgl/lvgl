/**
 * @file lv_draw_sdl_refr.h
 *
 */

#ifndef LV_TEMPL_H
#define LV_TEMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sdl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct _lv_draw_sdl_layer_ctx_t {
    lv_draw_layer_ctx_t base;

    SDL_Texture * orig_target;
    SDL_Texture * target;
    SDL_Rect target_rect;
    lv_draw_layer_flags_t flags;
} lv_draw_sdl_layer_ctx_t;
/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_draw_layer_ctx_t * lv_draw_sdl_layer_init(lv_draw_ctx_t * draw_ctx, lv_draw_layer_ctx_t * layer_ctx,
                                             lv_draw_layer_flags_t flags);

void lv_draw_sdl_layer_blend(lv_draw_ctx_t * draw_ctx, lv_draw_layer_ctx_t * transform_ctx,
                             const lv_draw_img_dsc_t * draw_dsc);

void lv_draw_sdl_layer_destroy(lv_draw_ctx_t * draw_ctx, lv_draw_layer_ctx_t * layer_ctx);

void lv_draw_sdl_transform_areas_offset(lv_draw_sdl_ctx_t * ctx, bool has_composite, lv_area_t * apply_area,
                                        lv_area_t * coords, lv_area_t * clip);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEMPL_H*/
