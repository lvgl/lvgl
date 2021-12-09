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

#include LV_DRAW_SDL_INCLUDE_PATH

#include "../../misc/lv_area.h"
#include "../../misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

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
bool lv_draw_sdl_mask_begin(const lv_area_t *coords_in, const lv_area_t *clip_in, const lv_area_t *extension,
                            lv_area_t *coords_out, lv_area_t *clip_out, lv_area_t *apply_area);

void lv_draw_sdl_mask_end(const lv_area_t *apply_area);

lv_opa_t * lv_draw_mask_dump(const lv_area_t * coords, const int16_t * ids, int16_t ids_count);

SDL_Surface * lv_sdl_create_mask_surface(lv_opa_t * pixels, lv_coord_t width, lv_coord_t height, lv_coord_t stride);

SDL_Texture * lv_sdl_create_mask_texture(SDL_Renderer * renderer, lv_opa_t * pixels, lv_coord_t width,
                                         lv_coord_t height, lv_coord_t stride);

SDL_Surface * lv_sdl_apply_mask_surface(const lv_area_t * coords, const int16_t * ids, int16_t ids_count);

SDL_Texture *
lv_sdl_gen_mask_texture(SDL_Renderer * renderer, const lv_area_t * coords, const int16_t * ids, int16_t ids_count);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SDL_MASK_H*/
