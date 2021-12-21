/**
 * @file lv_draw_sdl_utils.h
 *
 */
#ifndef LV_DRAW_SDL_UTILS_H
#define LV_DRAW_SDL_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_GPU_SDL

#include "lv_draw_sdl.h"
#include "../../misc/lv_color.h"
#include "../../misc/lv_area.h"

#include LV_GPU_SDL_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void _lv_draw_sdl_utils_init();

void _lv_draw_sdl_utils_deinit();

void lv_area_to_sdl_rect(const lv_area_t * in, SDL_Rect * out);

void lv_color_to_sdl_color(const lv_color_t * in, SDL_Color * out);

void lv_area_zoom_to_sdl_rect(const lv_area_t * in, SDL_Rect * out, uint16_t zoom, const lv_point_t * pivot);

SDL_Palette * lv_sdl_alloc_palette_for_bpp(const uint8_t * mapping, uint8_t bpp);

SDL_Surface * lv_sdl_create_opa_surface(lv_opa_t * opa, lv_coord_t width, lv_coord_t height, lv_coord_t stride);

SDL_Texture * lv_sdl_create_opa_texture(SDL_Renderer * renderer, lv_opa_t * pixels, lv_coord_t width,
                                        lv_coord_t height, lv_coord_t stride);

void lv_sdl_to_8bpp(uint8_t * dest, const uint8_t * src, int width, int height, int stride, uint8_t bpp);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_SDL*/
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SDL_UTILS_H*/
