/**
 * @file lv_gpu_sdl_mask.h
 *
 */

#ifndef LV_GPU_SDL_MASK_H
#define LV_GPU_SDL_MASK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#include LV_GPU_SDL_INCLUDE_PATH

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

#endif /*LV_GPU_SDL_MASK_H*/
