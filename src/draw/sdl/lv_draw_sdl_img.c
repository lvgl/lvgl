/**
 * @file lv_draw_sdl_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "../lv_draw_img.h"
#include "../lv_img_cache.h"
#include "../lv_draw_mask.h"
#include "../../misc/lv_lru.h"
#include "../../misc/lv_gc.h"

#include "lv_draw_sdl_img.h"
#include "lv_draw_sdl_utils.h"
#include "lv_draw_sdl_texture_cache.h"
#include "lv_draw_sdl_composite.h"
#include "lv_draw_sdl_mask.h"
#include "lv_draw_sdl_rect.h"

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

static bool check_mask_simple_radius(lv_coord_t * radius);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_res_t lv_draw_sdl_img_core(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * draw_dsc,
                              const lv_area_t * coords, const void * src)
{
    const lv_area_t * clip = draw_ctx->clip_area;
    lv_draw_sdl_ctx_t * ctx = (lv_draw_sdl_ctx_t *) draw_ctx;
    SDL_Renderer * renderer = ctx->renderer;

    size_t key_size;
    lv_draw_sdl_cache_key_head_img_t * key = lv_draw_sdl_texture_img_key_create(src, draw_dsc->frame_id, &key_size);
    bool texture_found = false;
    lv_draw_sdl_img_header_t * header = NULL;
    SDL_Texture * texture = lv_draw_sdl_texture_cache_get_with_userdata(ctx, key, key_size, &texture_found,
                                                                        (void **) &header);
    if(!texture_found) {
        lv_draw_sdl_img_load_texture(ctx, key, key_size, src, draw_dsc->frame_id, &texture, &header);
    }
    SDL_free(key);
    if(!texture) {
        return LV_RES_INV;
    }

    lv_area_t zoomed_cords;
    _lv_img_buf_get_transformed_area(&zoomed_cords, lv_area_get_width(coords), lv_area_get_height(coords), 0,
                                     draw_dsc->zoom, &draw_dsc->pivot);
    lv_area_move(&zoomed_cords, coords->x1, coords->y1);

    /* When in > 0, draw simple radius */
    lv_coord_t radius = 0;
    /* Coords will be translated so coords will start at (0,0) */
    lv_area_t t_coords = zoomed_cords, t_clip = *clip, apply_area;

    if(!check_mask_simple_radius(&radius)) {
        lv_draw_sdl_composite_begin(ctx, &zoomed_cords, clip, NULL, draw_dsc->blend_mode,
                                    &t_coords, &t_clip, &apply_area);
    }

    SDL_Rect clip_rect, coords_rect;
    lv_area_to_sdl_rect(&t_clip, &clip_rect);
    lv_area_to_sdl_rect(&t_coords, &coords_rect);

    SDL_Point pivot = {.x = draw_dsc->pivot.x, .y = draw_dsc->pivot.y};
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_Rect clipped_src = {0, 0, 0, 0}, clipped_dst = coords_rect;
    SDL_Rect * src_rect = NULL, *dst_rect = &clipped_dst;

    bool needs_clip = false;
    if(_lv_area_is_in(&t_coords, &t_clip, 0)) {
        /*Image needs to be fully drawn*/
        src_rect = SDL_RectEmpty(&header->rect) ? NULL : &header->rect;
    }
    else if(draw_dsc->angle == 0) {
        /*Image needs to be partly drawn, and we calculate the area to draw manually*/
        Uint32 format = 0;
        int access = 0, w, h;
        if(SDL_RectEmpty(&header->rect)) {
            SDL_QueryTexture(texture, &format, &access, &w, &h);
        }
        else {
            w = header->rect.w;
            h = header->rect.h;
        }
        SDL_IntersectRect(&clip_rect, &coords_rect, &clipped_dst);
        clipped_src.x = header->rect.x + (clipped_dst.x - coords_rect.x) * w / coords_rect.w;
        clipped_src.y = header->rect.y + (clipped_dst.y - coords_rect.y) * h / coords_rect.h;
        clipped_src.w = w - (coords_rect.w - clipped_dst.w) * w / coords_rect.w;
        clipped_src.h = h - (coords_rect.h - clipped_dst.h) * h / coords_rect.h;
        src_rect = &clipped_src;
    }
    else {
        /*Image needs to be rotated, so we have to use clip rect which is slower*/
        needs_clip = true;
    }

    SDL_Point dst_origin = {dst_rect->x, dst_rect->y};
    SDL_Texture * composite = NULL, *old_target = NULL;
    if(radius > 0) {
        dst_rect->x = 0;
        dst_rect->y = 0;
        lv_coord_t real_radius = LV_MIN3(radius, coords_rect.w, coords_rect.h);
        SDL_Texture * radius_frag = lv_draw_sdl_rect_bg_frag_obtain(ctx, real_radius);
        composite = lv_draw_sdl_composite_texture_obtain(ctx, LV_DRAW_SDL_COMPOSITE_TEXTURE_ID_TARGET1, dst_rect->w,
                                                         dst_rect->h);
        old_target = SDL_GetRenderTarget(renderer);
        SDL_SetRenderTarget(renderer, composite);
        /* First we create an opaque canvas */
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        /* Then we make 4 rounded corners */
        SDL_SetTextureBlendMode(radius_frag, SDL_BLENDMODE_NONE);
        lv_area_t mask_coords = t_coords, mask_clip = t_clip;
        lv_coord_t x_ofs = -mask_coords.x1 - (dst_origin.x - zoomed_cords.x1),
                   y_ofs = -mask_coords.y1 - (dst_origin.y - zoomed_cords.y1);
        lv_area_move(&mask_clip, x_ofs, y_ofs);
        lv_area_move(&mask_coords, x_ofs, y_ofs);
        lv_draw_sdl_rect_bg_frag_draw_corners(ctx, radius_frag, real_radius, &mask_coords, &mask_clip);
    }
    else if(needs_clip) {
        /* No radius, set clip here */
        SDL_RenderSetClipRect(renderer, &clip_rect);
    }

    /* Draw with no recolor */
    if(draw_dsc->recolor_opa > LV_OPA_TRANSP) {
        lv_color_t recolor = lv_color_mix(draw_dsc->recolor, lv_color_white(), draw_dsc->recolor_opa);
        SDL_SetTextureColorMod(texture, recolor.ch.red, recolor.ch.green, recolor.ch.blue);
    }
    else {
        SDL_SetTextureColorMod(texture, 0xFF, 0xFF, 0xFF);
    }
    SDL_SetTextureAlphaMod(texture, draw_dsc->opa);
    if(composite) {
#if LV_GPU_SDL_CUSTOM_BLEND_MODE
        SDL_BlendMode blend_mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_DST_COLOR, SDL_BLENDFACTOR_ZERO,
                                                              SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_DST_ALPHA,
                                                              SDL_BLENDFACTOR_ZERO, SDL_BLENDOPERATION_ADD);
        SDL_SetTextureBlendMode(texture, blend_mode);
#else
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_MOD);
#endif
    }
    SDL_RenderCopyEx(renderer, texture, src_rect, dst_rect, draw_dsc->angle, &pivot, SDL_FLIP_NONE);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    if(composite) {
        if(needs_clip) {
            SDL_RenderSetClipRect(renderer, &clip_rect);
        }
        SDL_SetRenderTarget(renderer, old_target);
        dst_rect->x = dst_origin.x;
        dst_rect->y = dst_origin.y;
        SDL_Rect composite_src = {0, 0, dst_rect->w, dst_rect->h};
        SDL_SetTextureBlendMode(composite, SDL_BLENDMODE_BLEND);
        SDL_RenderCopy(renderer, composite, &composite_src, dst_rect);
    }
    SDL_RenderSetClipRect(renderer, NULL);

    lv_draw_sdl_composite_end(ctx, &apply_area, draw_dsc->blend_mode);

    return LV_RES_OK;
}

bool lv_draw_sdl_img_load_texture(lv_draw_sdl_ctx_t * ctx, lv_draw_sdl_cache_key_head_img_t * key, size_t key_size,
                                  const void * src, int32_t frame_id, SDL_Texture ** texture,
                                  lv_draw_sdl_img_header_t ** header)
{
    _lv_img_cache_entry_t * cdsc = _lv_img_cache_open(src, lv_color_white(), frame_id);
    lv_draw_sdl_cache_flag_t tex_flags = 0;
    SDL_Rect rect;
    SDL_memset(&rect, 0, sizeof(SDL_Rect));
    if(cdsc) {
        lv_img_decoder_dsc_t * dsc = &cdsc->dec_dsc;
        if(dsc->user_data && SDL_memcmp(dsc->user_data, LV_DRAW_SDL_DEC_DSC_TEXTURE_HEAD, 8) == 0) {
            lv_draw_sdl_dec_dsc_userdata_t * ptr = (lv_draw_sdl_dec_dsc_userdata_t *) dsc->user_data;
            *texture = ptr->texture;
            rect = ptr->rect;
            if(ptr->texture_managed) {
                tex_flags |= LV_DRAW_SDL_CACHE_FLAG_MANAGED;
            }
            ptr->texture_referenced = true;
        }
        else {
            *texture = upload_img_texture(ctx->renderer, dsc);
        }
#if LV_IMG_CACHE_DEF_SIZE == 0
        lv_img_decoder_close(dsc);
#endif
    }
    if(texture && cdsc) {
        *header = SDL_malloc(sizeof(lv_draw_sdl_img_header_t));
        SDL_memcpy(&(*header)->base, &cdsc->dec_dsc.header, sizeof(lv_img_header_t));
        (*header)->rect = rect;
        lv_draw_sdl_texture_cache_put_advanced(ctx, key, key_size, *texture, *header, SDL_free, tex_flags);
    }
    else {
        lv_draw_sdl_texture_cache_put(ctx, key, key_size, NULL);
        return false;
    }
    return true;
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

/**
 * Check if there is only one radius mask
 * @param radius Set to radius value if the only mask is a radius mask
 * @return true if the only mask is a radius mask
 */
static bool check_mask_simple_radius(lv_coord_t * radius)
{
    if(lv_draw_mask_get_cnt() != 1) return false;
    for(uint8_t i = 0; i < _LV_MASK_MAX_NUM; i++) {
        _lv_draw_mask_common_dsc_t * param = LV_GC_ROOT(_lv_draw_mask_list[i]).param;
        if(param->type == LV_DRAW_MASK_TYPE_RADIUS) {
            lv_draw_mask_radius_param_t * rparam = (lv_draw_mask_radius_param_t *) param;
            if(rparam->cfg.outer) return false;
            *radius = rparam->cfg.radius;
            return true;
        }
    }
    return false;
}

#endif /*LV_USE_GPU_SDL*/
