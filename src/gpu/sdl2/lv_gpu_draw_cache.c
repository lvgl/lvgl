//
// Created by Mariotaku on 2021/08/21.
//

#include "draw/lv_draw_mask.h"
#include "misc/lv_color.h"
#include "misc/lv_mem.h"
#include "lv_gpu_draw_cache.h"
#include <SDL.h>

lv_lru_t *lv_sdl2_texture_cache;
static SDL_Palette *lv_sdl2_palette_grayscale8;

void lv_gpu_draw_cache_init() {
    lv_sdl2_texture_cache = lv_lru_new(1024 * 1024 * 128, 65536, (lv_lru_free_t *) SDL_DestroyTexture, free);
    lv_sdl2_palette_grayscale8 = SDL_AllocPalette(256);
    SDL_Color palette[256];
    for (int i = 0; i < 256; i++) {
        palette[i].r = palette[i].g = palette[i].b = 0xFF;
        palette[i].a = i;
    }
    SDL_SetPaletteColors(lv_sdl2_palette_grayscale8, palette, 0, 256);
}

void lv_gpu_draw_cache_deinit() {
    SDL_FreePalette(lv_sdl2_palette_grayscale8);
    lv_lru_free(lv_sdl2_texture_cache);
}

SDL_Surface *lv_sdl2_create_mask_surface(lv_opa_t *pixels, lv_coord_t width, lv_coord_t height) {
    SDL_Surface *indexed = SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, 8,
                                                              width, SDL_PIXELFORMAT_INDEX8);
    SDL_SetSurfacePalette(indexed, lv_sdl2_palette_grayscale8);
    SDL_Surface *converted = SDL_ConvertSurfaceFormat(indexed, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(indexed);
    return converted;
}

SDL_Surface *lv_sdl2_apply_mask_surface(const lv_area_t *coords, const SDL_Rect *coords_rect) {
    lv_opa_t *mask_buf = lv_mem_buf_get(coords_rect->w * coords_rect->h);
    for (lv_coord_t y = 0; y < coords_rect->h; y++) {
        lv_memset_ff(&mask_buf[y * coords_rect->w], coords_rect->w);
        lv_draw_mask_res_t res = lv_draw_mask_apply(&mask_buf[y * coords_rect->w], coords->x1,
                                                    (lv_coord_t) (y + coords->y1),
                                                    (lv_coord_t) coords_rect->w);
        if (res == LV_DRAW_MASK_RES_TRANSP) {
            lv_memset_00(&mask_buf[y * coords_rect->w], coords_rect->w);
        }
    }

    lv_mem_buf_release(mask_buf);
    return lv_sdl2_create_mask_surface(mask_buf, coords_rect->w, coords_rect->h);
}

SDL_Texture *lv_sdl2_gen_mask_texture(SDL_Renderer *renderer, const lv_area_t *coords, const SDL_Rect *coords_rect) {
    SDL_Surface *indexed = lv_sdl2_apply_mask_surface(coords, coords_rect);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, indexed);
    SDL_FreeSurface(indexed);
    return texture;
}