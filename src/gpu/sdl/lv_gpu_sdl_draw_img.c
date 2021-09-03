/**
 * @file lv_gpu_sdl_draw_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "../../core/lv_refr.h"
#include "lv_gpu_sdl_utils.h"
#include "lv_gpu_sdl_lru.h"
#include "lv_gpu_sdl_texture_cache.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_gpu_cache_key_magic_t magic;
    const void *src;
    int32_t frame_id;
} lv_draw_img_key_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static SDL_Texture *upload_img_texture(SDL_Renderer *renderer, lv_img_decoder_dsc_t *dsc);

static SDL_Texture *upload_img_texture_fallback(SDL_Renderer *renderer, lv_img_decoder_dsc_t *dsc);

static lv_draw_img_key_t img_key_create(const void *src, int32_t frame_id);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_img(const lv_area_t *coords, const lv_area_t *mask, const void *src, const lv_draw_img_dsc_t *draw_dsc) {
    if (draw_dsc->opa <= LV_OPA_MIN) return;

    lv_disp_t *disp = _lv_refr_get_disp_refreshing();
    SDL_Renderer *renderer = (SDL_Renderer *) disp->driver->user_data;

    lv_draw_img_key_t key = img_key_create(src, draw_dsc->frame_id);
    bool texture_found = false;
    SDL_Texture *texture = lv_gpu_draw_cache_get(&key, sizeof(key), &texture_found);
    if (!texture_found) {
        _lv_img_cache_entry_t *cdsc = _lv_img_cache_open(src, draw_dsc->recolor, draw_dsc->frame_id);
        if (cdsc) {
            texture = upload_img_texture(renderer, &cdsc->dec_dsc);
#if LV_IMG_CACHE_DEF_SIZE == 0
            lv_img_decoder_close(&cdsc->dec_dsc);
#endif
        }
        lv_gpu_draw_cache_put(&key, sizeof(key), texture);
    }
    if (!texture) {
        return;
    }

    SDL_Rect mask_rect, coords_rect;
    lv_area_to_sdl_rect(mask, &mask_rect);
    lv_area_to_sdl_rect(coords, &coords_rect);
    lv_area_zoom_to_sdl_rect(coords, &coords_rect, draw_dsc->zoom, &draw_dsc->pivot);

    SDL_Point pivot = {.x = draw_dsc->pivot.x, .y = draw_dsc->pivot.y};
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

/**********************
 *   STATIC FUNCTIONS
 **********************/

static SDL_Texture *upload_img_texture(SDL_Renderer *renderer, lv_img_decoder_dsc_t *dsc) {
    if (!dsc->img_data) {
        return upload_img_texture_fallback(renderer, dsc);
    }
    int chroma_keyed = dsc->header.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED;
    int w = dsc->header.w, h = dsc->header.h;
    void *data = (void *) dsc->img_data;
    Uint32 rmask = 0x00FF0000, gmask = 0x0000FF00, bmask = 0x000000FF, amask = 0xFF000000;
    if (chroma_keyed) {
        amask = 0x00;
    }
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(data, w, h, LV_COLOR_DEPTH, w * LV_COLOR_DEPTH / 8,
                                                    rmask, gmask, bmask, amask);
    SDL_SetColorKey(surface, chroma_keyed, lv_color_to32(LV_COLOR_CHROMA_KEY));
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

static SDL_Texture *upload_img_texture_fallback(SDL_Renderer *renderer, lv_img_decoder_dsc_t *dsc) {
    lv_coord_t w = dsc->header.w, h = dsc->header.h;
    uint8_t *data = lv_mem_buf_get(w * h * sizeof(lv_color_t));
    for (lv_coord_t y = 0; y < h; y++) {
        lv_img_decoder_read_line(dsc, 0, y, w, &data[y * w * sizeof(lv_color_t)]);
    }
    Uint32 rmask = 0x00FF0000, gmask = 0x0000FF00, bmask = 0x000000FF, amask = 0xFF000000;
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(data, w, h, LV_COLOR_DEPTH, w * LV_COLOR_DEPTH / 8,
                                                    rmask, gmask, bmask, amask);
    SDL_SetColorKey(surface, SDL_TRUE, lv_color_to32(LV_COLOR_CHROMA_KEY));
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    lv_mem_buf_release(data);
    return texture;
}

static lv_draw_img_key_t img_key_create(const void *src, int32_t frame_id) {
    lv_draw_img_key_t key;
    /* VERY IMPORTANT! Padding between members is uninitialized, so we have to wipe them manually */
    SDL_memset(&key, 0, sizeof(key));
    key.magic = LV_GPU_CACHE_KEY_MAGIC_IMG;
    key.src = src;
    key.frame_id = frame_id;
    return key;
}

#endif /*LV_USE_GPU_SDL*/
