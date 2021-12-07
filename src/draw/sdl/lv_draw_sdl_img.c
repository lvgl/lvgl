/**
 * @file lv_draw_sdl_draw_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_SDL

#include "../lv_draw_img.h"
#include "../lv_img_cache.h"
#include "../lv_draw_mask.h"
#include "../../misc/lv_lru.h"

#include "lv_draw_sdl_utils.h"
#include "lv_draw_sdl_texture_cache.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/


/**********************
 *  STATIC PROTOTYPES
 **********************/

static SDL_Texture * upload_img_texture(SDL_Renderer * renderer, lv_img_decoder_dsc_t * dsc);

static SDL_Texture * upload_img_texture_fallback(SDL_Renderer * renderer, lv_img_decoder_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_res_t lv_draw_sdl_img_core(const lv_area_t * coords, const lv_area_t * mask, const void * src,
                              const lv_draw_img_dsc_t * draw_dsc)
{
    lv_draw_sdl_context_t * ctx = lv_draw_sdl_get_context();
    SDL_Renderer * renderer = ctx->renderer;

    size_t key_size;
    lv_draw_sdl_cache_key_head_img_t * key = lv_draw_sdl_texture_img_key_create(src, draw_dsc->frame_id, &key_size);
    bool texture_found = false;
    SDL_Texture * texture = lv_draw_sdl_texture_cache_get(key, key_size, &texture_found);
    if(!texture_found) {
        _lv_img_cache_entry_t * cdsc = _lv_img_cache_open(src, draw_dsc->recolor, draw_dsc->frame_id);
        lv_draw_sdl_cache_flag_t tex_flags = 0;
        if(cdsc) {
            lv_img_decoder_dsc_t * dsc = &cdsc->dec_dsc;
            if(dsc->user_data && SDL_memcmp(dsc->user_data, LV_DRAW_SDL_DEC_DSC_TEXTURE_HEAD, 8) == 0) {
                lv_draw_sdl_dec_dsc_userdata_t *ptr = (lv_draw_sdl_dec_dsc_userdata_t *) dsc->user_data;
                texture = ptr->texture;
                if (ptr->texture_managed) {
                    tex_flags |= LV_DRAW_SDL_CACHE_FLAG_MANAGED;
                }
                ptr->texture_referenced = true;
            }
            else {
                texture = upload_img_texture(renderer, dsc);
            }
#if LV_IMG_CACHE_DEF_SIZE == 0
            lv_img_decoder_close(dsc);
#endif
        }
        if(texture && cdsc) {
            lv_img_header_t * header = SDL_malloc(sizeof(lv_img_header_t));
            SDL_memcpy(header, &cdsc->dec_dsc.header, sizeof(lv_img_header_t));
            lv_draw_sdl_texture_cache_put_advanced(key, key_size, texture, header, SDL_free, tex_flags);
        }
        else {
            lv_draw_sdl_texture_cache_put(key, key_size, NULL);
        }
    }
    SDL_free(key);
    if(!texture) {
        return LV_RES_INV;
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
    /*Draw original image if not fully recolored*/
    /*TODO: what if the image is translucent as well?*/
    if(draw_dsc->recolor_opa < LV_OPA_MAX) {
        SDL_RenderCopyEx(renderer, texture, NULL, &coords_rect, draw_dsc->angle, &pivot, SDL_FLIP_NONE);
    }

    SDL_SetTextureColorMod(texture, recolor.r, recolor.g, recolor.b);
    if(draw_dsc->recolor_opa >= LV_OPA_MAX) {
        /*Draw fully colored image*/
        SDL_SetTextureAlphaMod(texture, draw_dsc->opa);
        SDL_RenderCopyEx(renderer, texture, NULL, &coords_rect, draw_dsc->angle, &pivot, SDL_FLIP_NONE);
    }
    else if(draw_dsc->recolor_opa >= LV_OPA_MIN) {
        SDL_SetTextureAlphaMod(texture, draw_dsc->recolor_opa);
        SDL_RenderCopyEx(renderer, texture, NULL, &coords_rect, draw_dsc->angle, &pivot, SDL_FLIP_NONE);
    }
    SDL_RenderSetClipRect(renderer, NULL);
    return LV_RES_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static SDL_Texture * upload_img_texture(SDL_Renderer * renderer, lv_img_decoder_dsc_t * dsc)
{
    if(!dsc->img_data) {
        return upload_img_texture_fallback(renderer, dsc);
    }
    bool chroma_keyed = dsc->header.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED;
    uint32_t h = dsc->header.h;
    uint32_t w = dsc->header.w;
    void * data = (void *) dsc->img_data;
    Uint32 rmask = 0x00FF0000;
    Uint32 gmask = 0x0000FF00;
    Uint32 bmask = 0x000000FF;
    Uint32 amask = 0xFF000000;
    if(chroma_keyed) {
        amask = 0x00;
    }
    SDL_Surface * surface = SDL_CreateRGBSurfaceFrom(data, w, h, LV_COLOR_DEPTH, w * LV_COLOR_DEPTH / 8,
                                                     rmask, gmask, bmask, amask);
    SDL_SetColorKey(surface, chroma_keyed, lv_color_to32(LV_COLOR_CHROMA_KEY));
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

static SDL_Texture * upload_img_texture_fallback(SDL_Renderer * renderer, lv_img_decoder_dsc_t * dsc)
{
    lv_coord_t h = dsc->header.h;
    lv_coord_t w = dsc->header.w;
    uint8_t * data = lv_mem_buf_get(w * h * sizeof(lv_color_t));
    for(lv_coord_t y = 0; y < h; y++) {
        lv_img_decoder_read_line(dsc, 0, y, w, &data[y * w * sizeof(lv_color_t)]);
    }
    Uint32 rmask = 0x00FF0000;
    Uint32 gmask = 0x0000FF00;
    Uint32 bmask = 0x000000FF;
    Uint32 amask = 0xFF000000;
    SDL_Surface * surface = SDL_CreateRGBSurfaceFrom(data, w, h, LV_COLOR_DEPTH, w * LV_COLOR_DEPTH / 8,
                                                     rmask, gmask, bmask, amask);
    SDL_SetColorKey(surface, SDL_TRUE, lv_color_to32(LV_COLOR_CHROMA_KEY));
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    lv_mem_buf_release(data);
    return texture;
}


#endif /*LV_USE_DRAW_SDL*/
