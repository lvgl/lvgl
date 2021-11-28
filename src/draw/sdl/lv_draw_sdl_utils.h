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

#include "lvgl/src/misc/lv_color.h"
#include "lvgl/src/misc/lv_area.h"

#include LV_GPU_SDL_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct lv_draw_sdl_backend_context_t {
    SDL_Renderer * renderer;
    SDL_Texture * texture;
} lv_draw_sdl_backend_context_t;
/**********************
 * GLOBAL PROTOTYPES
 **********************/

void _lv_draw_sdl_utils_init();

void _lv_draw_sdl_utils_deinit();

void lv_area_to_sdl_rect(const lv_area_t * in, SDL_Rect * out);

void lv_color_to_sdl_color(const lv_color_t * in, SDL_Color * out);

void lv_area_zoom_to_sdl_rect(const lv_area_t * in, SDL_Rect * out, uint16_t zoom, const lv_point_t * pivot);

double lv_sdl_round(double d);

SDL_Palette * lv_sdl_alloc_palette_for_bpp(const uint8_t * mapping, uint8_t bpp);

SDL_Palette * lv_sdl_get_grayscale_palette(uint8_t bpp);

void lv_sdl_to_8bpp(uint8_t * dest, const uint8_t * src, int width, int height, int stride, uint8_t bpp);

lv_draw_sdl_backend_context_t * lv_draw_sdl_get_context();

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SDL_UTILS_H*/
