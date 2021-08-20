//
// Created by Mariotaku on 2021/08/21.
//

#ifndef LVGL_SDL_EXAMPLE_LV_GPU_DRAW_CACHE_H
#define LVGL_SDL_EXAMPLE_LV_GPU_DRAW_CACHE_H

#include "SDL.h"
#include "misc/lv_area.h"
#include "lv_gpu_sdl2_lru.h"

extern lv_lru_t *lv_sdl2_texture_cache;

void lv_gpu_draw_cache_init();

void lv_gpu_draw_cache_deinit();

SDL_Surface *lv_sdl2_create_mask_surface(lv_opa_t *pixels, lv_coord_t width, lv_coord_t height);

SDL_Surface *lv_sdl2_apply_mask_surface(const lv_area_t *coords, const SDL_Rect *coords_rect);

SDL_Texture *lv_sdl2_gen_mask_texture(SDL_Renderer *renderer, const lv_area_t *coords, const SDL_Rect *coords_rect);

#endif //LVGL_SDL_EXAMPLE_LV_GPU_DRAW_CACHE_H
