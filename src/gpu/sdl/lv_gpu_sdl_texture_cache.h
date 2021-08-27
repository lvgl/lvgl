//
// Created by Mariotaku on 2021/08/21.
//

#ifndef LVGL_SDL_EXAMPLE_LV_GPU_SDL_TEXTURE_CACHE_H
#define LVGL_SDL_EXAMPLE_LV_GPU_SDL_TEXTURE_CACHE_H

#include <SDL.h>
#include "../../misc/lv_area.h"
#include "lv_gpu_sdl_lru.h"

typedef enum {
    LV_GPU_CACHE_KEY_MAGIC_ARC = 0x01,
    LV_GPU_CACHE_KEY_MAGIC_IMG = 0x11,
    LV_GPU_CACHE_KEY_MAGIC_LINE = 0x21,
    LV_GPU_CACHE_KEY_MAGIC_RECT_BG = 0x31,
    LV_GPU_CACHE_KEY_MAGIC_RECT_SHADOW = 0x32,
    LV_GPU_CACHE_KEY_MAGIC_RECT_BORDER = 0x33,
    LV_GPU_CACHE_KEY_MAGIC_FONT = 0x41,
    LV_GPU_CACHE_KEY_TEMP = 0xFF,
} lv_gpu_cache_key_magic_t;

void _lv_gpu_sdl_texture_cache_init();

void _lv_gpu_sdl_texture_cache_deinit();

SDL_Texture *lv_gpu_draw_cache_get(const void *key, size_t key_length, bool *found);

SDL_Texture *lv_gpu_draw_cache_get_with_userdata(const void *key, size_t key_length, bool *found, void **userdata);

void lv_gpu_draw_cache_put(const void *key, size_t key_length, SDL_Texture *texture);

void lv_gpu_draw_cache_put_with_userdata(const void *key, size_t key_length, SDL_Texture *texture, void *userdata,
                                         lv_lru_free_t userdata_free);

SDL_Texture *lv_gpu_temp_texture_obtain(SDL_Renderer *renderer, lv_coord_t width, lv_coord_t height);

#endif //LVGL_SDL_EXAMPLE_LV_GPU_SDL_TEXTURE_CACHE_H
