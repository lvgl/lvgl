#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include <SDL.h>
#include "draw/lv_draw_label.h"

#include "lv_gpu_draw_cache.h"
#include "lv_gpu_sdl2_mask.h"
#include "lv_gpu_sdl2_utils.h"

static lv_lru_t *lv_sdl2_texture_cache;

typedef struct {
    SDL_Texture *texture;
} draw_cache_value_t;

static void draw_cache_free_value(draw_cache_value_t *);

void lv_gpu_draw_cache_init() {
    lv_sdl2_texture_cache = lv_lru_new(1024 * 1024 * 128, 65536, (lv_lru_free_t *) draw_cache_free_value, free);
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

SDL_Texture *lv_gpu_draw_cache_get(const void *key, size_t key_length, bool *found) {
    draw_cache_value_t *value = NULL;
    lv_lru_get(lv_sdl2_texture_cache, key, key_length, (void **) &value);
    if (!value) {
        if (found) {
            *found = false;
        }
        return NULL;
    }
    if (found) {
        *found = true;
    }
    return value->texture;
}

void lv_gpu_draw_cache_put(const void *key, size_t key_length, SDL_Texture *texture) {
    draw_cache_value_t *value = malloc(sizeof(draw_cache_value_t));
    value->texture = texture;
    if (!texture) {
        lv_lru_set(lv_sdl2_texture_cache, key, key_length, value, 1);
        return;
    }
    Uint32 format;
    int access, width, height;
    if (SDL_QueryTexture(texture, &format, &access, &width, &height) != 0) {
        return;
    }
    lv_lru_set(lv_sdl2_texture_cache, key, key_length, value, width * height * SDL_BITSPERPIXEL(format) / 8);
}

static void draw_cache_free_value(draw_cache_value_t *value) {
    if (value->texture) {
        SDL_DestroyTexture(value->texture);
    }
    free(value);
}

#endif /*LV_USE_GPU_SDL*/