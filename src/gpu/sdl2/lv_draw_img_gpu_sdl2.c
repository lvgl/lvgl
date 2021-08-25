//
// Created by Mariotaku on 2021/08/22.
//
#include "core/lv_refr.h"
#include "gpu/lv_gpu_sdl.h"
#include "lv_gpu_sdl2_utils.h"
#include "lv_gpu_sdl2_lru.h"
#include "lv_gpu_draw_cache.h"

typedef struct {
    lv_gpu_cache_key_magic_t magic;
    const void *src;
    int32_t frame_id;
} lv_draw_img_key_t;

void lv_draw_img(const lv_area_t *coords, const lv_area_t *mask, const void *src, const lv_draw_img_dsc_t *draw_dsc) {


    if (draw_dsc->opa <= LV_OPA_MIN) return;

    _lv_img_cache_entry_t *cdsc = _lv_img_cache_open(src, draw_dsc->recolor, draw_dsc->frame_id);

    if (cdsc == NULL) return;
    SDL_PixelFormatEnum pixel_format;
    int chroma_keyed = SDL_FALSE;
    switch (cdsc->dec_dsc.header.cf) {
        case LV_IMG_CF_TRUE_COLOR_ALPHA: {
            pixel_format = SDL_PIXELFORMAT_ARGB8888;
            break;
        }
        case LV_IMG_CF_TRUE_COLOR: {
            pixel_format = SDL_PIXELFORMAT_XRGB8888;
            break;
        }
        case LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED: {
            pixel_format = SDL_PIXELFORMAT_XRGB8888;
            chroma_keyed = SDL_TRUE;
            break;
        }
        default: {
//            SDL_assert(cdsc->dec_dsc.header.cf != 0);
            // Unsupported color format
            return;
        }
    }

    lv_disp_t *disp = _lv_refr_get_disp_refreshing();
    SDL_Renderer *renderer = (SDL_Renderer *) disp->driver->user_data;

    SDL_Rect mask_rect, coords_rect;
    lv_area_to_sdl_rect(mask, &mask_rect);
    lv_area_to_sdl_rect(coords, &coords_rect);
    lv_area_zoom_to_sdl_rect(coords, &coords_rect, draw_dsc->zoom, &draw_dsc->pivot);

    lv_draw_img_key_t key = {.magic=LV_GPU_CACHE_KEY_MAGIC_IMG, .src = src, .frame_id = draw_dsc->frame_id};
    SDL_Texture *texture = lv_gpu_draw_cache_get(&key, sizeof(key));
    if (!texture) {
        uint32_t w = cdsc->dec_dsc.header.w, h = cdsc->dec_dsc.header.h;

        SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom((void *) cdsc->dec_dsc.img_data, w, h,
                                                                  SDL_BITSPERPIXEL(pixel_format),
                                                                  w * sizeof(lv_color_t), pixel_format);
        SDL_SetColorKey(surface, chroma_keyed, lv_color_to32(LV_COLOR_CHROMA_KEY));
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        lv_gpu_draw_cache_put(&key, sizeof(key), texture);
    }
    SDL_Point pivot = {.x = coords_rect.w / 2, .y = coords_rect.h / 2};
    SDL_SetTextureAlphaMod(texture, draw_dsc->opa);
    SDL_SetTextureColorMod(texture, 0xFF, 0xFF, 0xFF);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_RenderSetClipRect(renderer, &mask_rect);

    SDL_Color recolor;
    lv_color_to_sdl_color(&draw_dsc->recolor, &recolor);
    // Draw original image if not fully recolored
    // TODO: what if the image is translucent as well?
    if (draw_dsc->recolor_opa < LV_OPA_MAX) {
        SDL_RenderCopyEx(renderer, texture, NULL, &coords_rect, draw_dsc->angle, &pivot, SDL_FLIP_NONE);
    }

    SDL_SetTextureColorMod(texture, recolor.r, recolor.g, recolor.b);
    if (draw_dsc->recolor_opa >= LV_OPA_MAX) {
        // Draw fully colored image
        SDL_SetTextureAlphaMod(texture, draw_dsc->opa);
        SDL_RenderCopyEx(renderer, texture, NULL, &coords_rect, draw_dsc->angle, &pivot, SDL_FLIP_NONE);
    } else if (draw_dsc->recolor_opa >= LV_OPA_MIN) {
        SDL_SetTextureAlphaMod(texture, draw_dsc->recolor_opa);
        SDL_RenderCopyEx(renderer, texture, NULL, &coords_rect, draw_dsc->angle, &pivot, SDL_FLIP_NONE);
    }
}