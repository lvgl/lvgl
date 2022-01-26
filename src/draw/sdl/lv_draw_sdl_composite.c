/**
 * @file lv_draw_sdl_composite.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "../../misc/lv_gc.h"
#include "../../core/lv_refr.h"
#include "lv_draw_sdl_composite.h"
#include "lv_draw_sdl_mask.h"
#include "lv_draw_sdl_utils.h"
#include "lv_draw_sdl_priv.h"
#include "lv_draw_sdl_texture_cache.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_sdl_cache_key_magic_t magic;
    lv_draw_sdl_composite_texture_id_t type;
} composite_key_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static composite_key_t mask_key_create(lv_draw_sdl_composite_texture_id_t type);

static lv_coord_t next_pow_of_2(lv_coord_t num);

static void dump_masks(SDL_Texture * texture, const lv_area_t * coords);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_draw_sdl_composite_begin(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords_in, const lv_area_t * clip_in,
                                 const lv_area_t * extension, lv_blend_mode_t blend_mode, lv_area_t * coords_out,
                                 lv_area_t * clip_out, lv_area_t * apply_area)
{
    lv_area_t full_coords = *coords_in;

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

    if(!_lv_area_intersect(apply_area, &full_coords, clip_in)) return false;
    bool has_mask = lv_draw_mask_is_any(apply_area);

    const bool draw_mask = has_mask && LV_GPU_SDL_CUSTOM_BLEND_MODE;
    const bool draw_blend = blend_mode != LV_BLEND_MODE_NORMAL;
    if(draw_mask || draw_blend) {
        lv_draw_sdl_context_internals_t * internals = ctx->internals;
        LV_ASSERT(internals->mask == NULL && internals->composition == NULL);

        lv_coord_t w = lv_area_get_width(apply_area), h = lv_area_get_height(apply_area);
        internals->composition = lv_draw_sdl_composite_texture_obtain(ctx, LV_DRAW_SDL_COMPOSITE_TEXTURE_ID_TARGET0, w, h);
        /* Don't need to worry about overflow */
        lv_coord_t ofs_x = (lv_coord_t) - apply_area->x1, ofs_y = (lv_coord_t) - apply_area->y1;
        /* Offset draw area to start with (0,0) of coords */
        lv_area_move(coords_out, ofs_x, ofs_y);
        lv_area_move(clip_out, ofs_x, ofs_y);
        SDL_SetRenderTarget(ctx->renderer, internals->composition);
        SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 0);
        SDL_RenderClear(ctx->renderer);
#if LV_GPU_SDL_CUSTOM_BLEND_MODE
        internals->mask = lv_draw_sdl_composite_texture_obtain(ctx, LV_DRAW_SDL_COMPOSITE_TEXTURE_ID_STREAM0, w, h);
        dump_masks(internals->mask, apply_area);
#endif
    }
    else if(has_mask) {
        /* Fallback mask handling. This will at least make bars looks less bad */
        for(uint8_t i = 0; i < _LV_MASK_MAX_NUM; i++) {
            _lv_draw_mask_common_dsc_t * comm_param = LV_GC_ROOT(_lv_draw_mask_list[i]).param;
            if(comm_param == NULL) continue;
            switch(comm_param->type) {
                case LV_DRAW_MASK_TYPE_RADIUS: {
                        const lv_draw_mask_radius_param_t * param = (const lv_draw_mask_radius_param_t *) comm_param;
                        if(param->cfg.outer) break;
                        _lv_area_intersect(clip_out, apply_area, &param->cfg.rect);
                        break;
                    }
                default:
                    break;
            }
        }
    }
    return has_mask;
}

void lv_draw_sdl_composite_end(lv_draw_sdl_ctx_t * ctx, const lv_area_t * apply_area, lv_blend_mode_t blend_mode)
{
    lv_draw_sdl_context_internals_t * internals = ctx->internals;
    SDL_Rect src_rect = {0, 0, lv_area_get_width(apply_area), lv_area_get_height(apply_area)};
#if LV_GPU_SDL_CUSTOM_BLEND_MODE
    if(internals->mask) {
        SDL_BlendMode mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE,
                                                        SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ZERO,
                                                        SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDOPERATION_ADD);
        SDL_SetTextureBlendMode(internals->mask, mode);
        SDL_RenderCopy(ctx->renderer, internals->mask, &src_rect, &src_rect);
    }
#endif

    /* Shapes are drawn on composite layer when mask or blend mode is present */
    if(internals->composition) {
        SDL_Rect dst_rect;
        lv_area_to_sdl_rect(apply_area, &dst_rect);

        SDL_SetRenderTarget(ctx->renderer, ctx->base_draw.buf);
        switch(blend_mode) {
            case LV_BLEND_MODE_NORMAL:
                SDL_SetTextureBlendMode(internals->composition, SDL_BLENDMODE_BLEND);
                break;
            case LV_BLEND_MODE_ADDITIVE:
                SDL_SetTextureBlendMode(internals->composition, SDL_BLENDMODE_ADD);
                break;
#if LV_GPU_SDL_CUSTOM_BLEND_MODE
            case LV_BLEND_MODE_SUBTRACTIVE: {
                    SDL_BlendMode mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE,
                                                                    SDL_BLENDOPERATION_SUBTRACT, SDL_BLENDFACTOR_ONE,
                                                                    SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_SUBTRACT);
                    SDL_SetTextureBlendMode(internals->composition, mode);
                    break;
                }
            case LV_BLEND_MODE_MULTIPLY: {
                    SDL_BlendMode mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_SRC_COLOR,
                                                                    SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ZERO,
                                                                    SDL_BLENDFACTOR_DST_ALPHA, SDL_BLENDOPERATION_ADD);
                    SDL_SetTextureBlendMode(internals->composition, mode);
                    break;
                }
#endif
            default:
                LV_LOG_WARN("Doesn't support blend mode %d", blend_mode);
                SDL_SetTextureBlendMode(internals->composition, SDL_BLENDMODE_BLEND);
                /* Unsupported yet */
                break;
        }
        SDL_RenderCopy(ctx->renderer, internals->composition, &src_rect, &dst_rect);
    }

    internals->mask = internals->composition = NULL;
}

SDL_Texture * lv_draw_sdl_composite_texture_obtain(lv_draw_sdl_ctx_t * ctx, lv_draw_sdl_composite_texture_id_t id,
                                                   lv_coord_t w, lv_coord_t h)
{
    lv_point_t * tex_size = NULL;
    composite_key_t mask_key = mask_key_create(id);
    SDL_Texture * result = lv_draw_sdl_texture_cache_get_with_userdata(ctx, &mask_key, sizeof(composite_key_t), NULL,
                                                                       (void **) &tex_size);
    if(!result || tex_size->x < w || tex_size->y < h) {
        lv_coord_t size = next_pow_of_2(LV_MAX(w, h));
        int access = SDL_TEXTUREACCESS_STREAMING;
        if(id >= LV_DRAW_SDL_COMPOSITE_TEXTURE_ID_TARGET0) {
            access = SDL_TEXTUREACCESS_TARGET;
        }
        result = SDL_CreateTexture(ctx->renderer, LV_DRAW_SDL_TEXTURE_FORMAT, access, size, size);
        tex_size = lv_mem_alloc(sizeof(lv_point_t));
        tex_size->x = tex_size->y = size;
        lv_draw_sdl_texture_cache_put_advanced(ctx, &mask_key, sizeof(composite_key_t), result, tex_size, lv_mem_free, 0);
    }
    return result;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static composite_key_t mask_key_create(lv_draw_sdl_composite_texture_id_t type)
{
    composite_key_t key;
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

static void dump_masks(SDL_Texture * texture, const lv_area_t * coords)
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
        res = lv_draw_mask_apply(line_buf, abs_x, abs_y, len);
        if(res == LV_DRAW_MASK_RES_TRANSP) {
            lv_memset_00(&pixels[y * pitch], 4 * rect.w);
        }
        else if(res == LV_DRAW_MASK_RES_FULL_COVER) {
            lv_memset_ff(&pixels[y * pitch], 4 * rect.w);
        }
        else {
            for(int x = 0; x < rect.w; x++) {
                const size_t idx = y * pitch + x * 4;
                pixels[idx] = line_buf[x];
                pixels[idx + 1] = pixels[idx + 2] = pixels[idx + 3] = 0xFF;
            }
        }
    }
    lv_mem_buf_release(line_buf);
    SDL_UnlockTexture(texture);
}

#endif /*LV_USE_GPU_SDL*/
