//
// Created by Mariotaku on 2021/08/21.
//

#ifndef LVGL_SDL_EXAMPLE_LV_GPU_DRAW_CACHE_H
#define LVGL_SDL_EXAMPLE_LV_GPU_DRAW_CACHE_H

#include "SDL.h"
#include "misc/lv_area.h"
#include "lv_gpu_sdl2_lru.h"

typedef uint8_t lv_gpu_cache_key_magic_t;

#define LV_GPU_CACHE_KEY_MAGIC_ARC 0x01
#define LV_GPU_CACHE_KEY_MAGIC_IMG 0x11
#define LV_GPU_CACHE_KEY_MAGIC_LINE 0x21
#define LV_GPU_CACHE_KEY_MAGIC_RECT_BG 0x31
#define LV_GPU_CACHE_KEY_MAGIC_RECT_SHADOW 0x3A
#define LV_GPU_CACHE_KEY_MAGIC_RECT_BORDER 0x3B

void lv_gpu_draw_cache_init();

void lv_gpu_draw_cache_deinit();

SDL_Texture *lv_gpu_draw_cache_get(const void *key, size_t key_length, bool *found);

void lv_gpu_draw_cache_put(const void *key, size_t key_length, SDL_Texture *texture);

#endif //LVGL_SDL_EXAMPLE_LV_GPU_DRAW_CACHE_H
