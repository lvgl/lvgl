//
// Created by Mariotaku on 2021/08/21.
//

#ifndef LV_GPU_SDL2_UTILS_H
#define LV_GPU_SDL2_UTILS_H

#include "misc/lv_color.h"
#include "misc/lv_area.h"
#include "SDL_rect.h"

void _lv_gpu_sdl_utils_init();

void _lv_gpu_sdl_utils_deinit();

void lv_area_to_sdl_rect(const lv_area_t *in, SDL_Rect *out);

void lv_color_to_sdl_color(const lv_color_t *in, SDL_Color *out);

void lv_area_zoom_to_sdl_rect(const lv_area_t *in, SDL_Rect *out, uint16_t zoom, const lv_point_t *pivot);

double lv_sdl_round(double d);

SDL_Palette *lv_sdl_alloc_palette_for_bpp(const uint8_t *mapping, uint8_t bpp);

SDL_Palette *lv_sdl_get_grayscale_palette(uint8_t bpp);

void lv_sdl_to_8bpp(uint8_t *dest, const uint8_t *src, int width, int height, int stride, uint8_t bpp);

#endif //LV_GPU_SDL2_UTILS_H
