/**
 * @file lv_draw_sdl_mask.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "../../misc/lv_gc.h"
#include "../../core/lv_refr.h"
#include "lv_draw_sdl_mask.h"
#include "lv_draw_sdl_utils.h"
#include "lv_draw_sdl_priv.h"
#include "lv_draw_sdl_texture_cache.h"

/*********************
 *      DEFINES
 *********************/
#define HAS_CUSTOM_BLEND_MODE (SDL_VERSION_ATLEAST(2, 0, 6))

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_sdl_cache_key_magic_t magic;
    lv_draw_sdl_mask_cache_type_t type;
} lv_mask_key_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_mask_key_t mask_key_create(lv_draw_sdl_mask_cache_type_t type);

static lv_coord_t next_pow_of_2(lv_coord_t num);

void texture_apply_mask(SDL_Texture * texture, const lv_area_t * coords, const int16_t * ids, int16_t ids_count);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_draw_sdl_mask_begin(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords_in, const lv_area_t * clip_in,
                            const lv_area_t * extension, lv_area_t * coords_out, lv_area_t * clip_out,
                            lv_area_t * apply_area)
{
    lv_area_t full_area, full_coords = *coords_in;

    /* Normalize full_coords */
    if(full_coords.x1 > full_coords.x2) {
        lv_coord_t x2 = full_coords.x2;
        full_coords.x2 = full_coords.x1;
        full_coords.x1 = x2;
    }
    if(full_coords.y1 > full_coords.y2) {
        lv_coord_t y2 = full_coords.y2;
        full_coords.y2 = full_coords.y1;
        full_coords.y1 = y2;
    }

    if(extension) {
        full_coords.x1 -= extension->x1;
        full_coords.x2 += extension->x2;
        full_coords.y1 -= extension->y1;
        full_coords.y2 += extension->y2;
    }

    if(!_lv_area_intersect(&full_area, &full_coords, clip_in)) return false;
    if(!lv_draw_mask_is_any(&full_area)) return false;
#if HAS_CUSTOM_BLEND_MODE

    lv_draw_sdl_context_internals_t * internals = ctx->internals;
    LV_ASSERT(internals->mask == NULL && internals->composition == NULL);
    lv_coord_t w = lv_area_get_width(&full_area), h = lv_area_get_height(&full_area);


    internals->mask = lv_draw_sdl_mask_tmp_obtain(ctx, LV_DRAW_SDL_MASK_KEY_ID_MASK, w, h);
    texture_apply_mask(internals->mask, &full_area, NULL, 0);

    internals->composition = lv_draw_sdl_mask_tmp_obtain(ctx, LV_DRAW_SDL_MASK_KEY_ID_COMPOSITE, w, h);

    *apply_area = full_area;
    /* Don't need to worry about overflow */
    lv_coord_t ofs_x = (lv_coord_t) - full_area.x1, ofs_y = (lv_coord_t) - full_area.y1;
    /* Offset draw area to start with (0,0) of coords */
    lv_area_move(&full_area, ofs_x, ofs_y);
    lv_area_move(coords_out, ofs_x, ofs_y);
    lv_area_move(clip_out, ofs_x, ofs_y);
    SDL_SetRenderTarget(ctx->renderer, internals->composition);
    SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 0);
    SDL_RenderClear(ctx->renderer);
#else
    /* Fallback mask handling. This will at least make bars looks less bad */
    for(uint8_t i = 0; i < _LV_MASK_MAX_NUM; i++) {
        _lv_draw_mask_common_dsc_t * comm_param = LV_GC_ROOT(_lv_draw_mask_list[i]).param;
        if(comm_param == NULL) continue;
        switch(comm_param->type) {
            case LV_DRAW_MASK_TYPE_RADIUS: {
                    const lv_draw_mask_radius_param_t * param = (const lv_draw_mask_radius_param_t *) comm_param;
                    if(param->cfg.outer) break;
                    _lv_area_intersect(clip_out, &full_area, &param->cfg.rect);
                    break;
                }
            default:
                break;
        }
    }
#endif
    return true;
}

void lv_draw_sdl_mask_end(lv_draw_sdl_ctx_t * ctx, const lv_area_t * apply_area)
{
#if HAS_CUSTOM_BLEND_MODE
    lv_draw_sdl_context_internals_t * internals = ctx->internals;
    LV_ASSERT(internals->mask != NULL && internals->composition != NULL);
    SDL_BlendMode mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE,
                                                    SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ZERO,
                                                    SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDOPERATION_ADD);
    SDL_SetTextureBlendMode(internals->mask, mode);
    SDL_Rect src_rect = {0, 0, lv_area_get_width(apply_area), lv_area_get_height(apply_area)};
    SDL_RenderCopy(ctx->renderer, internals->mask, &src_rect, &src_rect);

    SDL_Rect dst_rect;
    lv_area_to_sdl_rect(apply_area, &dst_rect);

    SDL_SetRenderTarget(ctx->renderer, ctx->base_draw.base_draw.buf);
    SDL_SetTextureBlendMode(internals->composition, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(ctx->renderer, internals->composition, &src_rect, &dst_rect);

    internals->mask = internals->composition = NULL;
#endif
}

lv_opa_t * lv_draw_sdl_mask_dump_opa(const lv_area_t * coords, const int16_t * ids, int16_t ids_count)
{
    SDL_assert(coords->x2 >= coords->x1);
    SDL_assert(coords->y2 >= coords->y1);
    lv_coord_t w = lv_area_get_width(coords), h = lv_area_get_height(coords);
    lv_opa_t * mask_buf = lv_mem_buf_get(w * h);
    for(lv_coord_t y = 0; y < h; y++) {
        lv_opa_t * line_buf = &mask_buf[y * w];
        lv_memset_ff(line_buf, w);
        lv_coord_t abs_x = (lv_coord_t) coords->x1, abs_y = (lv_coord_t)(y + coords->y1), len = (lv_coord_t) w;
        lv_draw_mask_res_t res;
        if(ids) {
            res = lv_draw_mask_apply_ids(line_buf, abs_x, abs_y, len, ids, ids_count);
        }
        else {
            res = lv_draw_mask_apply(line_buf, abs_x, abs_y, len);
        }
        if(res == LV_DRAW_MASK_RES_TRANSP) {
            lv_memset_00(line_buf, w);
        }
    }
    return mask_buf;
}

SDL_Texture * lv_draw_sdl_mask_dump_texture(SDL_Renderer * renderer, const lv_area_t * coords, const int16_t * ids,
                                            int16_t ids_count)
{
    lv_coord_t w = lv_area_get_width(coords), h = lv_area_get_height(coords);
    lv_opa_t * mask_buf = lv_draw_sdl_mask_dump_opa(coords, ids, ids_count);
    SDL_Surface * surface = lv_sdl_create_opa_surface(mask_buf, w, h, w);
    lv_mem_buf_release(mask_buf);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

SDL_Texture * lv_draw_sdl_mask_tmp_obtain(lv_draw_sdl_ctx_t * ctx, lv_draw_sdl_mask_cache_type_t type,
                                          lv_coord_t w, lv_coord_t h)
{
    lv_point_t * tex_size = NULL;
    lv_mask_key_t mask_key = mask_key_create(type);
    SDL_Texture * result = lv_draw_sdl_texture_cache_get_with_userdata(ctx, &mask_key, sizeof(lv_mask_key_t), NULL,
                                                                       (void **) &tex_size);
    if(!result || tex_size->x < w || tex_size->y < h) {
        lv_coord_t size = next_pow_of_2(LV_MAX(w, h));
        int access = SDL_TEXTUREACCESS_STREAMING;
        if(type == LV_DRAW_SDL_MASK_KEY_ID_COMPOSITE) {
            access = SDL_TEXTUREACCESS_TARGET;
        }
        result = SDL_CreateTexture(ctx->renderer, LV_DRAW_SDL_TEXTURE_FORMAT, access, size, size);
        tex_size = lv_mem_alloc(sizeof(lv_point_t));
        tex_size->x = tex_size->y = size;
        lv_draw_sdl_texture_cache_put_advanced(ctx, &mask_key, sizeof(lv_mask_key_t), result, tex_size, lv_mem_free, 0);
    }
    return result;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_mask_key_t mask_key_create(lv_draw_sdl_mask_cache_type_t type)
{
    lv_mask_key_t key;
    /* VERY IMPORTANT! Padding between members is uninitialized, so we have to wipe them manually */
    SDL_memset(&key, 0, sizeof(key));
    key.magic = LV_GPU_CACHE_KEY_MAGIC_MASK;
    key.type = type;
    return key;
}

static lv_coord_t next_pow_of_2(lv_coord_t num)
{
    lv_coord_t n = 128;
    while(n < num && n < 16384) {
        n = n << 1;
    }
    return n;
}

void texture_apply_mask(SDL_Texture * texture, const lv_area_t * coords, const int16_t * ids, int16_t ids_count)
{
    lv_coord_t w = lv_area_get_width(coords), h = lv_area_get_height(coords);
    SDL_assert(w > 0 && h > 0);
    SDL_Rect rect = {0, 0, w, h};
    uint8_t * pixels;
    int pitch;
    if(SDL_LockTexture(texture, &rect, (void **) &pixels, &pitch) != 0) return;

    lv_opa_t * line_buf = lv_mem_buf_get(rect.w);
    for(lv_coord_t y = 0; y < rect.h; y++) {
        lv_memset_ff(line_buf, rect.w);
        lv_coord_t abs_x = (lv_coord_t) coords->x1, abs_y = (lv_coord_t)(y + coords->y1), len = (lv_coord_t) rect.w;
        lv_draw_mask_res_t res;
        if(ids) {
            res = lv_draw_mask_apply_ids(line_buf, abs_x, abs_y, len, ids, ids_count);
        }
        else {
            res = lv_draw_mask_apply(line_buf, abs_x, abs_y, len);
        }
        if(res == LV_DRAW_MASK_RES_TRANSP) {
            lv_memset_00(&pixels[y * pitch], 4 * rect.w);
        }
        else if(res == LV_DRAW_MASK_RES_FULL_COVER) {
            lv_memset_ff(&pixels[y * pitch], 4 * rect.w);
        }
        else {
            for(int x = 0; x < rect.w; x++) {
                pixels[y * pitch + x * 4] = line_buf[x];
            }
        }
    }
    lv_mem_buf_release(line_buf);
    SDL_UnlockTexture(texture);
}

#endif /*LV_USE_GPU_SDL*/
