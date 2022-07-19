/**
 * @file lv_draw_sdl_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_SDL

#include "lv_draw_sdl.h"
#include "lv_draw_sdl_utils.h"
#include "lv_draw_sdl_texture_cache.h"
#include "lv_draw_sdl_composite.h"
#include "lv_draw_sdl_mask.h"

/*********************
 *      DEFINES
 *********************/
#define ROUND_START 0x01
#define ROUND_END 0x02
/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_sdl_cache_key_magic_t magic;
    lv_coord_t length;
    lv_coord_t width;
    uint8_t round;
} lv_draw_line_key_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

static lv_draw_line_key_t line_key_create(const lv_draw_line_dsc_t * dsc, lv_coord_t length);

static SDL_Texture * line_texture_create(lv_draw_sdl_ctx_t * sdl_ctx, const lv_draw_line_dsc_t * dsc,
                                         lv_coord_t length);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_sdl_draw_line(lv_draw_ctx_t * draw_ctx, const lv_draw_line_dsc_t * dsc, const lv_point_t * point1,
                           const lv_point_t * point2)
{
    lv_draw_sdl_ctx_t * sdl_ctx = (lv_draw_sdl_ctx_t *) draw_ctx;
    SDL_Renderer * renderer = sdl_ctx->renderer;
    lv_coord_t x1 = point1->x, x2 = point2->x, y1 = point1->y, y2 = point2->y;
    double length = SDL_sqrt(SDL_pow(x2 - x1, 2) + SDL_pow(y2 - y1, 2));
    if(length - (long) length > 0.5) {
        length = (long) length + 1;
    }

    double angle = SDL_atan2(y2 - y1, x2 - x1) * 180 / M_PI;
    lv_draw_line_key_t key = line_key_create(dsc, (lv_coord_t) length);
    SDL_Texture * texture = lv_draw_sdl_texture_cache_get(sdl_ctx, &key, sizeof(key), NULL);
    if(!texture) {
        texture = line_texture_create(sdl_ctx, dsc, (lv_coord_t) length);
        lv_draw_sdl_texture_cache_put(sdl_ctx, &key, sizeof(key), texture);
    }

    lv_area_t coords = {x1, y1, x2, y2};
    const lv_area_t * clip = draw_ctx->clip_area;

    SDL_Rect coords_r, clip_r;
    lv_area_to_sdl_rect(&coords, &coords_r);
    lv_area_to_sdl_rect(clip, &clip_r);

    lv_area_t t_coords = coords, t_clip = *clip, apply_area;
    lv_area_t extension = {dsc->width / 2, dsc->width / 2, dsc->width / 2, dsc->width / 2};
    lv_draw_sdl_composite_begin(sdl_ctx, &coords, clip, &extension, dsc->blend_mode, &t_coords, &t_clip,
                                &apply_area);

    SDL_Color color;
    lv_color_to_sdl_color(&dsc->color, &color);

    SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(texture, dsc->opa);
    SDL_Rect srcrect = {0, 0, (int) length + dsc->width + 2, dsc->width + 2},
             dstrect = {t_coords.x1 - 1 - dsc->width / 2, t_coords.y1 - 1, srcrect.w, srcrect.h};
    SDL_Point center = {1 + dsc->width / 2, 1 + dsc->width / 2};

    SDL_Rect clip_rect;
    lv_area_to_sdl_rect(&t_clip, &clip_rect);
    if(!SDL_RectEquals(&clip_rect, &dstrect) || angle != 0) {
        SDL_RenderSetClipRect(renderer, &clip_rect);
    }
    SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, angle, &center, 0);
    SDL_RenderSetClipRect(renderer, NULL);

    lv_draw_sdl_composite_end(sdl_ctx, &apply_area, dsc->blend_mode);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_draw_line_key_t line_key_create(const lv_draw_line_dsc_t * dsc, lv_coord_t length)
{
    lv_draw_line_key_t key;
    lv_memzero(&key, sizeof(lv_draw_line_key_t));
    key.magic = LV_GPU_CACHE_KEY_MAGIC_LINE;
    key.length = length;
    key.width = dsc->width;
    key.round = (dsc->round_start ? ROUND_START : 0) | (dsc->round_end ? ROUND_END : 0);
    return key;
}

static SDL_Texture * line_texture_create(lv_draw_sdl_ctx_t * sdl_ctx, const lv_draw_line_dsc_t * dsc, lv_coord_t length)
{
    SDL_Texture * texture = SDL_CreateTexture(sdl_ctx->renderer, LV_DRAW_SDL_TEXTURE_FORMAT, SDL_TEXTUREACCESS_TARGET,
                                              length + dsc->width + 2, dsc->width + 2);
    SDL_Texture * target = SDL_GetRenderTarget(sdl_ctx->renderer);
    SDL_SetRenderTarget(sdl_ctx->renderer, texture);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(sdl_ctx->renderer, 0xFF, 0xFF, 0xFF, 0x0);
    SDL_RenderClear(sdl_ctx->renderer);
    SDL_SetRenderDrawColor(sdl_ctx->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_Rect line_rect = {1 + dsc->width / 2, 1, length, dsc->width};
    SDL_RenderFillRect(sdl_ctx->renderer, &line_rect);
    if(dsc->round_start || dsc->round_end) {
        lv_draw_mask_radius_param_t param;
        lv_area_t round_area = {0, 0, dsc->width - 1, dsc->width - 1};
        lv_draw_mask_radius_init(&param, &round_area, LV_RADIUS_CIRCLE, false);

        int16_t mask_id = lv_draw_mask_add(&param, NULL);
        SDL_Texture * round_texture = lv_draw_sdl_mask_dump_texture(sdl_ctx->renderer, &round_area, &mask_id, 1);
        lv_draw_mask_remove_id(mask_id);

        SDL_Rect round_src = {0, 0, dsc->width, dsc->width};
        SDL_Rect round_dst = {line_rect.x - dsc->width / 2, 1, dsc->width, dsc->width};
        SDL_RenderCopy(sdl_ctx->renderer, round_texture, &round_src, &round_dst);
        round_dst.x = line_rect.w + dsc->width / 2;
        SDL_RenderCopy(sdl_ctx->renderer, round_texture, &round_src, &round_dst);
        SDL_DestroyTexture(round_texture);
    }

    SDL_SetRenderTarget(sdl_ctx->renderer, target);
    return texture;
}

#endif /*LV_USE_DRAW_SDL*/
