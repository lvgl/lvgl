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
#include "lv_draw_sdl_rect.h"
#include "lv_draw_sdl_layer.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_sdl_cache_key_magic_t magic;
    const SDL_Texture * texture;
    lv_coord_t w, h, radius;
} lv_draw_img_rounded_key_t;

enum {
    ROUNDED_IMG_PART_LEFT = 0,
    ROUNDED_IMG_PART_HCENTER = 1,
    ROUNDED_IMG_PART_RIGHT = 2,
    ROUNDED_IMG_PART_TOP = 3,
    ROUNDED_IMG_PART_VCENTER = 4,
    ROUNDED_IMG_PART_BOTTOM = 5,
};

enum {
    ROUNDED_IMG_CORNER_TOP_LEFT = 0,
    ROUNDED_IMG_CORNER_TOP_RIGHT = 1,
    ROUNDED_IMG_CORNER_BOTTOM_RIGHT = 2,
    ROUNDED_IMG_CORNER_BOTTOM_LEFT = 3,
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static SDL_Texture * upload_img_texture(SDL_Renderer * renderer, lv_img_decoder_dsc_t * dsc);

static SDL_Texture * upload_img_texture_fallback(SDL_Renderer * renderer, lv_img_decoder_dsc_t * dsc);

static bool check_mask_simple_radius(const lv_area_t * coords, lv_coord_t * radius);

static void draw_img_simple(lv_draw_sdl_ctx_t * ctx, SDL_Texture * texture, const lv_draw_sdl_img_header_t * header,
                            const lv_draw_img_dsc_t * draw_dsc, const lv_area_t * coords, const lv_area_t * clip);

static void draw_img_rounded(lv_draw_sdl_ctx_t * ctx, SDL_Texture * texture, const lv_draw_sdl_img_header_t * header,
                             const lv_draw_img_dsc_t * draw_dsc, const lv_area_t * coords, const lv_area_t * clip,
                             lv_coord_t radius);

static SDL_Texture * img_rounded_frag_obtain(lv_draw_sdl_ctx_t * ctx, SDL_Texture * texture,
                                             const lv_draw_sdl_img_header_t * header, int w, int h, lv_coord_t radius);

static lv_draw_img_rounded_key_t rounded_key_create(const SDL_Texture * texture, lv_coord_t w, lv_coord_t h,
                                                    lv_coord_t radius);

static void calc_draw_part(SDL_Texture * texture, const lv_draw_sdl_img_header_t * header, const lv_area_t * coords,
                           const lv_area_t * clip, SDL_Rect * clipped_src, SDL_Rect * clipped_dst);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/


static void apply_recolor_opa(SDL_Texture * texture, const lv_draw_img_dsc_t * draw_dsc);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_res_t lv_draw_sdl_img_core(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * draw_dsc,
                              const lv_area_t * coords, const void * src)
{
    const lv_area_t * clip = draw_ctx->clip_area;
    lv_draw_sdl_ctx_t * ctx = (lv_draw_sdl_ctx_t *) draw_ctx;

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

    bool has_composite = false;

    if(!check_mask_simple_radius(&t_coords, &radius)) {
        has_composite = lv_draw_sdl_composite_begin(ctx, &zoomed_cords, clip, NULL, draw_dsc->blend_mode,
                                                    &t_coords, &t_clip, &apply_area);
    }

    lv_draw_sdl_transform_areas_offset(ctx, has_composite, &apply_area, &t_coords, &t_clip);

    SDL_Rect clip_rect, coords_rect;
    lv_area_to_sdl_rect(&t_clip, &clip_rect);
    lv_area_to_sdl_rect(&t_coords, &coords_rect);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    if(radius > 0) {
        draw_img_rounded(ctx, texture, header, draw_dsc, &t_coords, &t_clip, radius);
    }
    else {
        draw_img_simple(ctx, texture, header, draw_dsc, &t_coords, &t_clip);
    }

    lv_draw_sdl_composite_end(ctx, &apply_area, draw_dsc->blend_mode);

    return LV_RES_OK;
}

static void calc_draw_part(SDL_Texture * texture, const lv_draw_sdl_img_header_t * header, const lv_area_t * coords,
                           const lv_area_t * clip, SDL_Rect * clipped_src, SDL_Rect * clipped_dst)
{
    double x = 0, y = 0, w, h;
    if(SDL_RectEmpty(&header->rect)) {
        Uint32 format = 0;
        int access = 0, tw, th;
        SDL_QueryTexture(texture, &format, &access, &tw, &th);
        w = tw;
        h = th;
    }
    else {
        x = header->rect.x;
        y = header->rect.y;
        w = header->rect.w;
        h = header->rect.h;
    }
    if(clip) {
        lv_area_t clipped_area;
        _lv_area_intersect(&clipped_area, coords, clip);
        lv_area_to_sdl_rect(&clipped_area, clipped_dst);
    }
    else {
        lv_area_to_sdl_rect(coords, clipped_dst);
    }
    lv_coord_t coords_w = lv_area_get_width(coords), coords_h = lv_area_get_height(coords);
    clipped_src->x = (int)(x + (clipped_dst->x - coords->x1) * w / coords_w);
    clipped_src->y = (int)(y + (clipped_dst->y - coords->y1) * h / coords_h);
    clipped_src->w = (int)(w - (coords_w - clipped_dst->w) * w / coords_w);
    clipped_src->h = (int)(h - (coords_h - clipped_dst->h) * h / coords_h);
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
    bool chroma_keyed = dsc->header.cf == (uint32_t) LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED;
    int h = (int) dsc->header.h;
    int w = (int) dsc->header.w;
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
    lv_coord_t h = (lv_coord_t) dsc->header.h;
    lv_coord_t w = (lv_coord_t) dsc->header.w;
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
static bool check_mask_simple_radius(const lv_area_t * coords, lv_coord_t * radius)
{
    if(lv_draw_mask_get_cnt() != 1) return false;
    for(uint8_t i = 0; i < _LV_MASK_MAX_NUM; i++) {
        _lv_draw_mask_common_dsc_t * param = LV_GC_ROOT(_lv_draw_mask_list[i]).param;
        if(param->type == LV_DRAW_MASK_TYPE_RADIUS) {
            lv_draw_mask_radius_param_t * rparam = (lv_draw_mask_radius_param_t *) param;
            if(rparam->cfg.outer) return false;
            if(!_lv_area_is_equal(&rparam->cfg.rect, coords)) return false;
            *radius = rparam->cfg.radius;
            return true;
        }
    }
    return false;
}

static void draw_img_simple(lv_draw_sdl_ctx_t * ctx, SDL_Texture * texture, const lv_draw_sdl_img_header_t * header,
                            const lv_draw_img_dsc_t * draw_dsc, const lv_area_t * coords, const lv_area_t * clip)
{
    apply_recolor_opa(texture, draw_dsc);
    SDL_Point pivot = {.x = draw_dsc->pivot.x, .y = draw_dsc->pivot.y};

    /*Image needs to be rotated, so we have to use clip rect which is slower*/
    if(draw_dsc->angle != 0) {
        /* No radius, set clip here */
        SDL_Rect clip_rect;
        lv_area_to_sdl_rect(clip, &clip_rect);
        SDL_RenderSetClipRect(ctx->renderer, &clip_rect);
    }
    SDL_Rect src_rect, dst_rect;
    calc_draw_part(texture, header, coords, clip, &src_rect, &dst_rect);
    SDL_RenderCopyEx(ctx->renderer, texture, &src_rect, &dst_rect, draw_dsc->angle, &pivot, SDL_FLIP_NONE);
    if(draw_dsc->angle != 0) {
        SDL_RenderSetClipRect(ctx->renderer, NULL);
    }
}


static void draw_img_rounded(lv_draw_sdl_ctx_t * ctx, SDL_Texture * texture, const lv_draw_sdl_img_header_t * header,
                             const lv_draw_img_dsc_t * draw_dsc, const lv_area_t * coords, const lv_area_t * clip,
                             lv_coord_t radius)
{
    const int w = lv_area_get_width(coords), h = lv_area_get_height(coords);
    lv_coord_t real_radius = LV_MIN3(radius, w, h);
    SDL_Texture * frag = img_rounded_frag_obtain(ctx, texture, header, w, h, real_radius);
    apply_recolor_opa(frag, draw_dsc);
    lv_draw_sdl_rect_bg_frag_draw_corners(ctx, frag, real_radius, coords, clip, true);

    apply_recolor_opa(texture, draw_dsc);

    SDL_Rect src_rect, dst_rect;
    /* Draw 3 parts */
    lv_area_t clip_tmp, part;
    calc_draw_part(texture, header, coords, NULL, &src_rect, &dst_rect);
    for(int i = w > h ? ROUNDED_IMG_PART_LEFT : ROUNDED_IMG_PART_TOP, j = i + 3; i <= j; i++) {
        switch(i) {
            case ROUNDED_IMG_PART_LEFT:
                lv_area_set(&part, coords->x1, coords->y1 + radius, coords->x1 + radius - 1, coords->y2 - radius);
                break;
            case ROUNDED_IMG_PART_HCENTER:
                lv_area_set(&part, coords->x1 + radius, coords->y1, coords->x2 - radius, coords->y2);
                break;
            case ROUNDED_IMG_PART_RIGHT:
                lv_area_set(&part, coords->x2 - radius + 1, coords->y1 + radius, coords->x2, coords->y2 - radius);
                break;
            case ROUNDED_IMG_PART_TOP:
                lv_area_set(&part, coords->x1 + radius, coords->y1, coords->x2 - radius, coords->y1 + radius - 1);
                break;
            case ROUNDED_IMG_PART_VCENTER:
                lv_area_set(&part, coords->x1 + radius, coords->y2 - radius + 1, coords->x2 - radius, coords->y2);
                break;
            case ROUNDED_IMG_PART_BOTTOM:
                lv_area_set(&part, coords->x1, coords->y1 + radius, coords->x2, coords->y2 - radius);
                break;
            default:
                break;
        }
        if(!_lv_area_intersect(&clip_tmp, &part, clip)) continue;
        SDL_Rect clip_rect;
        lv_area_to_sdl_rect(&clip_tmp, &clip_rect);
        SDL_RenderSetClipRect(ctx->renderer, &clip_rect);
        SDL_RenderCopy(ctx->renderer, texture, &src_rect, &dst_rect);
    }
    SDL_RenderSetClipRect(ctx->renderer, NULL);
}

static void apply_recolor_opa(SDL_Texture * texture, const lv_draw_img_dsc_t * draw_dsc)
{
    if(draw_dsc->recolor_opa > LV_OPA_TRANSP) {
        /* Draw with mixed recolor */
        lv_color_t recolor = lv_color_mix(draw_dsc->recolor, lv_color_white(), draw_dsc->recolor_opa);
        SDL_SetTextureColorMod(texture, recolor.ch.red, recolor.ch.green, recolor.ch.blue);
    }
    else {
        /* Draw with no recolor */
        SDL_SetTextureColorMod(texture, 0xFF, 0xFF, 0xFF);
    }
    SDL_SetTextureAlphaMod(texture, draw_dsc->opa);
}

static SDL_Texture * img_rounded_frag_obtain(lv_draw_sdl_ctx_t * ctx, SDL_Texture * texture,
                                             const lv_draw_sdl_img_header_t * header, int w, int h, lv_coord_t radius)
{
    lv_draw_img_rounded_key_t key = rounded_key_create(texture, w, h, radius);
    SDL_Texture * mask_frag = lv_draw_sdl_rect_bg_frag_obtain(ctx, radius);
    SDL_Texture * img_frag = lv_draw_sdl_texture_cache_get(ctx, &key, sizeof(key), NULL);
    if(img_frag == NULL) {
        const lv_coord_t full_frag_size = radius * 2 + 3;
        img_frag = SDL_CreateTexture(ctx->renderer, LV_DRAW_SDL_TEXTURE_FORMAT, SDL_TEXTUREACCESS_TARGET,
                                     full_frag_size, full_frag_size);
        SDL_SetTextureBlendMode(img_frag, SDL_BLENDMODE_BLEND);
        SDL_Texture * old_target = SDL_GetRenderTarget(ctx->renderer);
        SDL_SetRenderTarget(ctx->renderer, img_frag);
        SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 0);
        /* SDL_RenderClear is not working properly, so we overwrite the target with solid color */
        SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_NONE);
        SDL_RenderFillRect(ctx->renderer, NULL);
        SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);

        lv_area_t coords = {0, 0, w - 1, h - 1}, clip;
        lv_area_t frag_coords = {0, 0, full_frag_size - 1, full_frag_size - 1};
        lv_draw_sdl_rect_bg_frag_draw_corners(ctx, mask_frag, radius, &frag_coords, NULL, false);

        SDL_SetTextureAlphaMod(texture, 0xFF);
        SDL_SetTextureColorMod(texture, 0xFF, 0xFF, 0xFF);
#if LV_GPU_SDL_CUSTOM_BLEND_MODE
        SDL_BlendMode blend_mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ZERO,
                                                              SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_DST_ALPHA,
                                                              SDL_BLENDFACTOR_ZERO, SDL_BLENDOPERATION_ADD);
        SDL_SetTextureBlendMode(texture, blend_mode);
#else
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_MOD);
#endif
        SDL_Rect srcrect, cliprect, dstrect = {0, 0, radius, radius};

        cliprect.w = cliprect.h = radius;
        for(int i = 0; i <= ROUNDED_IMG_CORNER_BOTTOM_LEFT; i++) {
            switch(i) {
                case ROUNDED_IMG_CORNER_TOP_LEFT:
                    cliprect.x = 0;
                    cliprect.y = 0;
                    lv_area_align(&frag_coords, &coords, LV_ALIGN_TOP_LEFT, 0, 0);
                    break;
                case ROUNDED_IMG_CORNER_TOP_RIGHT:
                    cliprect.x = full_frag_size - radius;
                    cliprect.y = 0;
                    lv_area_align(&frag_coords, &coords, LV_ALIGN_TOP_RIGHT, 0, 0);
                    break;
                case ROUNDED_IMG_CORNER_BOTTOM_RIGHT:
                    cliprect.x = full_frag_size - radius;
                    cliprect.y = full_frag_size - radius;
                    lv_area_align(&frag_coords, &coords, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
                    break;
                case ROUNDED_IMG_CORNER_BOTTOM_LEFT:
                    cliprect.x = 0;
                    cliprect.y = full_frag_size - radius;
                    lv_area_align(&frag_coords, &coords, LV_ALIGN_BOTTOM_LEFT, 0, 0);
                    break;
                default:
                    break;
            }
            calc_draw_part(texture, header, &coords, NULL, &srcrect, &dstrect);
            SDL_RenderSetClipRect(ctx->renderer, &cliprect);
            SDL_RenderCopy(ctx->renderer, texture, &srcrect, &dstrect);
        }
        SDL_RenderSetClipRect(ctx->renderer, NULL);

        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

        SDL_SetRenderTarget(ctx->renderer, old_target);
        lv_draw_sdl_texture_cache_put(ctx, &key, sizeof(key), img_frag);
    }
    return img_frag;
}

static lv_draw_img_rounded_key_t rounded_key_create(const SDL_Texture * texture, lv_coord_t w, lv_coord_t h,
                                                    lv_coord_t radius)
{
    lv_draw_img_rounded_key_t key;
    SDL_memset(&key, 0, sizeof(key));
    key.magic = LV_GPU_CACHE_KEY_MAGIC_IMG_ROUNDED_CORNERS;
    key.texture = texture;
    key.w = w;
    key.h = h;
    key.radius = radius;
    return key;
}

#endif /*LV_USE_GPU_SDL*/
