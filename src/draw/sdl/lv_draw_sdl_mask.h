/**
 * @file lv_draw_sdl_mask.h
 *
 */

#ifndef LV_DRAW_SDL_MASK_H
#define LV_DRAW_SDL_MASK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#include LV_GPU_SDL_INCLUDE_PATH

#include "lv_draw_sdl.h"
#include "../../misc/lv_area.h"
#include "../../misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum lv_draw_sdl_mask_cache_type_t {
    LV_DRAW_SDL_MASK_KEY_ID_MASK,
    LV_DRAW_SDL_MASK_KEY_ID_COMPOSITE,
} lv_draw_sdl_mask_cache_type_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Begin drawing with mask. Render target will be switched to a temporary texture,
 * and drawing coordinates may get clipped or translated
 * @param coords_in Original coordinates
 * @param clip_in Original clip area
 * @param extension Useful for shadows or outlines, can be NULL
 * @param coords_out Translated coords
 * @param clip_out Translated clip area
 * @param apply_area Area of actual composited texture will be drawn
 * @return true if there are any mask needs to be drawn, false otherwise
 */
bool lv_draw_sdl_mask_begin(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords_in, const lv_area_t * clip_in,
                            const lv_area_t * extension, lv_area_t * coords_out, lv_area_t * clip_out,
                            lv_area_t * apply_area);

void lv_draw_sdl_mask_end(lv_draw_sdl_ctx_t * ctx, const lv_area_t * apply_area);

lv_opa_t * lv_draw_sdl_mask_dump_opa(const lv_area_t * coords, const int16_t * ids, int16_t ids_count);

SDL_Texture * lv_draw_sdl_mask_dump_texture(SDL_Renderer * renderer, const lv_area_t * coords, const int16_t * ids,
                                            int16_t ids_count);

SDL_Texture * lv_draw_sdl_mask_tmp_obtain(lv_draw_sdl_ctx_t * ctx, lv_draw_sdl_mask_cache_type_t type,
                                          lv_coord_t w, lv_coord_t h);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SDL_MASK_H*/
