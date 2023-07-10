/**
 * @file lv_draw_sdl_composite.h
 *
 */

#ifndef LV_DRAW_SDL_COMPOSITE_H
#define LV_DRAW_SDL_COMPOSITE_H

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

typedef enum lv_draw_sdl_composite_texture_id_t {
    LV_DRAW_SDL_COMPOSITE_TEXTURE_ID_STREAM0,
    LV_DRAW_SDL_COMPOSITE_TEXTURE_ID_STREAM1,
    LV_DRAW_SDL_COMPOSITE_TEXTURE_ID_TARGET0,
    LV_DRAW_SDL_COMPOSITE_TEXTURE_ID_TARGET1,
    LV_DRAW_SDL_COMPOSITE_TEXTURE_ID_TRANSFORM0,
} lv_draw_sdl_composite_texture_id_t;

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
bool lv_draw_sdl_composite_begin(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords_in, const lv_area_t * clip_in,
                                 const lv_area_t * extension, lv_blend_mode_t blend_mode, lv_area_t * coords_out,
                                 lv_area_t * clip_out, lv_area_t * apply_area);

void lv_draw_sdl_composite_end(lv_draw_sdl_ctx_t * ctx, const lv_area_t * apply_area, lv_blend_mode_t blend_mode);

SDL_Texture * lv_draw_sdl_composite_texture_obtain(lv_draw_sdl_ctx_t * ctx, lv_draw_sdl_composite_texture_id_t id,
                                                   lv_coord_t w, lv_coord_t h);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SDL_COMPOSITE_H*/
