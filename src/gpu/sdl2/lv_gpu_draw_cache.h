//
// Created by Mariotaku on 2021/08/21.
//

#ifndef LVGL_SDL_EXAMPLE_LV_GPU_DRAW_CACHE_H
#define LVGL_SDL_EXAMPLE_LV_GPU_DRAW_CACHE_H

#include "SDL.h"
#include "misc/lv_area.h"
#include "lv_gpu_sdl2_lru.h"

void lv_gpu_draw_cache_init();

void lv_gpu_draw_cache_deinit();

SDL_Texture *lv_gpu_draw_cache_get(const void *key, size_t key_length);

void lv_gpu_draw_cache_put(const void *key, size_t key_length, SDL_Texture *texture);

#endif //LVGL_SDL_EXAMPLE_LV_GPU_DRAW_CACHE_H
