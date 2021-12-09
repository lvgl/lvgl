/**
 * @file lv_draw_sdl_mask.c
 *
 */

/*********************
 *      INCLUDES
 *********************/


#include <src/misc/lv_gc.h>
#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_SDL

#include "../../core/lv_refr.h"
#include "../lv_draw_mask.h"
#include "lv_draw_sdl_mask.h"
#include "lv_draw_sdl_utils.h"
#include "lv_draw_sdl_priv.h"
#include "lv_draw_sdl_texture_cache.h"

/*********************
 *      DEFINES
 *********************/
#define HAS_CUSTOM_BLEND_MODE (SDL_VERSION_ATLEAST(2, 0, 6))

#define KEY_ID_MASK 1
#define KEY_ID_COMPOSITE 2

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define MASK_PIXEL_FORMAT SDL_PIXELFORMAT_ARGB8888
#else
#define MASK_PIXEL_FORMAT SDL_PIXELFORMAT_RGBA8888
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_sdl_cache_key_magic_t magic;
    uint32_t id;
} lv_mask_key_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_mask_key_t mask_key_create(uint32_t id);

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

bool lv_draw_sdl_mask_begin(const lv_area_t *coords_in, const lv_area_t *clip_in, const lv_area_t *extension,
                            lv_area_t *coords_out, lv_area_t *clip_out, lv_area_t *apply_area) {
    lv_area_t full_area, full_coords = *coords_in;
    if (extension) {
        full_coords.x1 -= extension->x1;
        full_coords.x2 += extension->x2;
        full_coords.y1 -= extension->y1;
        full_coords.y2 += extension->y2;
    }
    if (!_lv_area_intersect(&full_area, &full_coords, clip_in)) return false;
    if (!lv_draw_mask_is_any(&full_area)) return false;
#if HAS_CUSTOM_BLEND_MODE
    lv_disp_t *disp = _lv_refr_get_disp_refreshing();

    lv_draw_sdl_context_t *context = disp->driver->user_data;
    lv_draw_sdl_context_internals_t *internals = context->internals;
    LV_ASSERT(internals->mask == NULL && internals->composition == NULL);
    lv_mask_key_t mask_key = mask_key_create(KEY_ID_MASK), composite_key = mask_key_create(KEY_ID_COMPOSITE);
    lv_coord_t w = lv_area_get_width(&full_area), h = lv_area_get_height(&full_area);

    lv_point_t *tex_size = NULL;
    internals->mask = lv_draw_sdl_texture_cache_get_with_userdata(&mask_key, sizeof(mask_key),
                                                                  NULL, (void **) &tex_size);
    if (!internals->mask || tex_size->x < w || tex_size->y < h) {
        lv_coord_t size = next_pow_of_2(LV_MAX(w, h));
        internals->mask = SDL_CreateTexture(context->renderer, MASK_PIXEL_FORMAT,
                                                   SDL_TEXTUREACCESS_STREAMING, size, size);
        tex_size = lv_mem_alloc(sizeof(lv_point_t));
        tex_size->x = tex_size->y = size;
        lv_draw_sdl_texture_cache_put_advanced(&mask_key, sizeof(mask_key), internals->mask,
                                               tex_size, lv_mem_free, 0);
    }
    texture_apply_mask(internals->mask, &full_area, NULL, 0);

    tex_size = NULL;
    internals->composition = lv_draw_sdl_texture_cache_get_with_userdata(&composite_key, sizeof(composite_key),
                                                                         NULL, (void **) &tex_size);
    if (!internals->composition || tex_size->x < w || tex_size->y < h) {
        lv_coord_t size = next_pow_of_2(LV_MAX(w, h));
        internals->composition = SDL_CreateTexture(context->renderer, MASK_PIXEL_FORMAT,
                                                   SDL_TEXTUREACCESS_TARGET, size, size);
        tex_size = lv_mem_alloc(sizeof(lv_point_t));
        tex_size->x = tex_size->y = size;
        lv_draw_sdl_texture_cache_put_advanced(&composite_key, sizeof(composite_key), internals->composition,
                                               tex_size, lv_mem_free, 0);
    }

    *apply_area = full_area;
    /* Don't need to worry about overflow */
    lv_coord_t ofs_x = (lv_coord_t) -full_area.x1, ofs_y = (lv_coord_t) -full_area.y1;
    /* Offset draw area to start with (0,0) of coords */
    lv_area_move(&full_area, ofs_x, ofs_y);
    lv_area_move(coords_out, ofs_x, ofs_y);
    lv_area_move(clip_out, ofs_x, ofs_y);
    SDL_SetRenderTarget(context->renderer, internals->composition);
    SDL_SetRenderDrawColor(context->renderer, 255, 255, 255, 0);
    SDL_RenderClear(context->renderer);
#else
    /* Fallback mask handling. This will at least make bars looks less bad */
    for (uint8_t i = 0; i < _LV_MASK_MAX_NUM; i++) {
        _lv_draw_mask_common_dsc_t *comm_param = LV_GC_ROOT(_lv_draw_mask_list[i]).param;
        if (comm_param == NULL) continue;
        switch (comm_param->type) {
            case LV_DRAW_MASK_TYPE_RADIUS: {
                const lv_draw_mask_radius_param_t *param = (const lv_draw_mask_radius_param_t *) comm_param;
                if (param->cfg.outer) break;
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

void lv_draw_sdl_mask_end(const lv_area_t *apply_area)
{
#if HAS_CUSTOM_BLEND_MODE
    lv_draw_sdl_context_t *context = lv_draw_sdl_get_context();
    lv_draw_sdl_context_internals_t *internals = context->internals;
    LV_ASSERT(internals->mask != NULL && internals->composition != NULL);
    SDL_BlendMode mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE,
                                                    SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ZERO,
                                                    SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDOPERATION_ADD);
    SDL_SetTextureBlendMode(internals->mask, mode);
    SDL_Rect src_rect = {0, 0, lv_area_get_width(apply_area), lv_area_get_height(apply_area)};
    SDL_RenderCopy(context->renderer, internals->mask, &src_rect, &src_rect);

    SDL_Rect dst_rect;
    lv_area_to_sdl_rect(apply_area, &dst_rect);

    SDL_SetRenderTarget(context->renderer, context->texture);
    SDL_SetTextureBlendMode(internals->composition, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(context->renderer, internals->composition, &src_rect, &dst_rect);

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
    SDL_Surface *surface = lv_sdl_create_opa_surface(mask_buf, w, h, w);
    lv_mem_buf_release(mask_buf);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_mask_key_t mask_key_create(uint32_t id)
{
    lv_mask_key_t key;
    /* VERY IMPORTANT! Padding between members is uninitialized, so we have to wipe them manually */
    SDL_memset(&key, 0, sizeof(key));
    key.magic = LV_GPU_CACHE_KEY_MAGIC_MASK;
    key.id = id;
    return key;
}

static lv_coord_t next_pow_of_2(lv_coord_t num)
{
    lv_coord_t n = 128;
    while (n < num && n < 16384) {
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
    if (SDL_LockTexture(texture, &rect, (void **) &pixels, &pitch) != 0) return;

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
        if (res == LV_DRAW_MASK_RES_TRANSP) {
            lv_memset_00(&pixels[y * pitch], 4 * rect.w);
        } else if (res == LV_DRAW_MASK_RES_FULL_COVER) {
            lv_memset_ff(&pixels[y * pitch], 4 * rect.w);
        } else {
            for (int x = 0; x < rect.w; x++) {
                pixels[y * pitch + x * 4] = line_buf[x];
            }
        }
    }
    lv_mem_buf_release(line_buf);
    SDL_UnlockTexture(texture);
}

#endif /*LV_USE_DRAW_SDL*/
