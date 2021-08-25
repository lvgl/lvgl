//
// Created by Mariotaku on 2021/08/21.
//

#include <SDL.h>
#include "draw/lv_draw_label.h"

#include "lv_gpu_draw_cache.h"
#include "lv_gpu_sdl2_mask.h"
#include "lv_gpu_sdl2_utils.h"

static lv_lru_t *lv_sdl2_texture_cache;

void lv_gpu_draw_cache_init() {
    lv_sdl2_texture_cache = lv_lru_new(1024 * 1024 * 128, 65536, (lv_lru_free_t *) SDL_DestroyTexture, free);
    lv_sdl2_palette_grayscale8 = lv_sdl_alloc_palette_for_bpp(_lv_bpp8_opa_table, 8);
    lv_sdl2_palette_grayscale4 = lv_sdl_alloc_palette_for_bpp(_lv_bpp4_opa_table, 4);
    lv_sdl2_palette_grayscale2 = lv_sdl_alloc_palette_for_bpp(_lv_bpp2_opa_table, 2);
    lv_sdl2_palette_grayscale1 = lv_sdl_alloc_palette_for_bpp(_lv_bpp1_opa_table, 1);
}

void lv_gpu_draw_cache_deinit() {
    SDL_FreePalette(lv_sdl2_palette_grayscale1);
    SDL_FreePalette(lv_sdl2_palette_grayscale2);
    SDL_FreePalette(lv_sdl2_palette_grayscale4);
    SDL_FreePalette(lv_sdl2_palette_grayscale8);
    lv_lru_free(lv_sdl2_texture_cache);
}

SDL_Texture *lv_gpu_draw_cache_get(const void *key, size_t key_length) {
    SDL_Texture *texture = NULL;
    lv_lru_get(lv_sdl2_texture_cache, key, key_length, (void **) &texture);
    return texture;
}

void lv_gpu_draw_cache_put(const void *key, size_t key_length, SDL_Texture *texture) {
    SDL_assert(texture);
    Uint32 format;
    int access, width, height;
    if (SDL_QueryTexture(texture, &format, &access, &width, &height) != 0) {
        return;
    }
    lv_lru_set(lv_sdl2_texture_cache, key, key_length, texture, width * height * SDL_BITSPERPIXEL(format) / 8);
}
