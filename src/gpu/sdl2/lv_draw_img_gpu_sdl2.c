//
// Created by Mariotaku on 2021/08/22.
//
#include "core/lv_refr.h"
#include "gpu/lv_gpu_sdl.h"
#include "lv_gpu_sdl2_utils.h"
#include "lv_gpu_sdl2_lru.h"
#include "lv_gpu_draw_cache.h"

typedef struct {
    const void *src;
    int32_t frame_id;
} lv_draw_img_key_t;

void lv_draw_img(const lv_area_t *coords, const lv_area_t *mask, const void *src, const lv_draw_img_dsc_t *draw_dsc) {


    if (draw_dsc->opa <= LV_OPA_MIN) return;

    _lv_img_cache_entry_t *cdsc = _lv_img_cache_open(src, draw_dsc->recolor, draw_dsc->frame_id);

    if (cdsc == NULL) return;

    lv_disp_t *disp = _lv_refr_get_disp_refreshing();
    SDL_Renderer *renderer = (SDL_Renderer *) lv_disp_get_draw_buf(disp)->buf_act;

    SDL_Rect mask_rect, coords_rect;
    lv_area_to_sdl_rect(mask, &mask_rect);
    lv_area_zoom_to_sdl_rect(coords, &coords_rect, draw_dsc->zoom, &draw_dsc->pivot);

    SDL_Texture *texture = NULL;
    lv_draw_img_key_t key = {.src = src, .frame_id = draw_dsc->frame_id};
    lv_lru_get(lv_sdl2_texture_cache, &key, sizeof(key), (void *) &texture);
    if (!texture) {
        int w = cdsc->dec_dsc.header.w, h = cdsc->dec_dsc.header.h;
        SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom((void *) cdsc->dec_dsc.img_data, w, h, 32,
                                                                  cdsc->dec_dsc.header.w * sizeof(lv_color_t),
                                                                  SDL_PIXELFORMAT_ARGB8888);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        lv_lru_set(lv_sdl2_texture_cache, &key, sizeof(key), texture, w * h);
    }
    SDL_SetTextureAlphaMod(texture, draw_dsc->opa);
    SDL_SetTextureColorMod(texture, 0xFF, 0xFF, 0xFF);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_Point pivot = {.x = coords_rect.w / 2, .y = coords_rect.h / 2};
    SDL_RenderSetClipRect(renderer, &mask_rect);
    SDL_RenderCopyEx(renderer, texture, NULL, &coords_rect, draw_dsc->angle, &pivot, SDL_FLIP_NONE);
}