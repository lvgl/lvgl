/**
 * @file lv_gpu_sdl_draw_line.c
 *
 * This implementation does not functioning properly so it's not enabled
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include LV_GPU_SDL_INCLUDE_PATH
#include "../../core/lv_refr.h"
#include "lv_gpu_sdl_utils.h"
#include "lv_gpu_sdl_lru.h"
#include "lv_gpu_sdl_texture_cache.h"
#include "lv_gpu_sdl_mask.h"

#ifndef M_PI
    #include <math.h>
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_gpu_cache_key_magic_t magic;
    lv_coord_t length;
    lv_coord_t thickness;
} lv_draw_line_key_t;

static lv_draw_line_key_t line_key_create(lv_coord_t length, lv_coord_t thickness);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_line2(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * clip,
                   const lv_draw_line_dsc_t * dsc)
{
    if(dsc->width == 0) return;
    if(dsc->opa <= LV_OPA_MIN) return;

    if(point1->x == point2->x && point1->y == point2->y) return;

    lv_area_t clip_line;
    clip_line.x1 = LV_MIN(point1->x, point2->x) - dsc->width / 2;
    clip_line.x2 = LV_MAX(point1->x, point2->x) + dsc->width / 2;
    clip_line.y1 = LV_MIN(point1->y, point2->y) - dsc->width / 2;
    clip_line.y2 = LV_MAX(point1->y, point2->y) + dsc->width / 2;

    if(!_lv_area_intersect(&clip_line, &clip_line, clip)) return;

    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    SDL_Renderer * renderer = (SDL_Renderer *) disp->driver->user_data;

    SDL_Color line_color;
    lv_color_to_sdl_color(&dsc->color, &line_color);

    int length = lv_sdl_round(SDL_sqrt(SDL_pow(point2->y - point1->y + 1, 2) + SDL_pow(point2->x - point1->x + 1, 2)));
    lv_coord_t thickness = dsc->width;
    lv_draw_line_key_t key = line_key_create(length, thickness);
    lv_area_t coords = {1, 1, length, dsc->width};
    lv_area_t tex_coords;
    lv_area_copy(&tex_coords, &coords);
    lv_area_increase(&tex_coords, 1, 1);

    SDL_Texture * texture = lv_gpu_draw_cache_get(&key, sizeof(key), NULL);

    if(texture == NULL) {
        lv_draw_mask_radius_param_t mask_rout_param;
        lv_draw_mask_radius_init(&mask_rout_param, &coords, 0, false);
        int16_t mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);
        texture = lv_sdl_gen_mask_texture(renderer, &tex_coords, &mask_rout_id, 1);
        lv_draw_mask_remove_id(mask_rout_id);
        SDL_assert(texture);
        lv_gpu_draw_cache_put(&key, sizeof(key), texture);
    }
    double angle = SDL_atan2(point2->y - point1->y, point2->x - point1->x) * 180 / M_PI;

    SDL_Rect clip_rect;
    lv_area_to_sdl_rect(&clip_line, &clip_rect);


    SDL_Texture * screen = SDL_GetRenderTarget(renderer);

    SDL_Texture * content = lv_gpu_temp_texture_obtain(renderer, clip_rect.w, clip_rect.h);
    SDL_SetTextureBlendMode(content, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, content);
    SDL_RenderSetClipRect(renderer, NULL);

    //    /* Replace texture with clip mask */
    SDL_Rect mask_rect = {.w = clip_rect.w, .h = clip_rect.h, .x = 0, .y = 0};
    //    SDL_Texture *mask = lv_sdl_gen_mask_texture(renderer, &clip_line, NULL, 0);
    //    SDL_SetTextureBlendMode(mask, SDL_BLENDMODE_NONE);
    ////    SDL_RenderCopy(renderer, mask, NULL, &mask_rect);
    //
    //    SDL_SetTextureAlphaMod(texture, 0xFF);
    //    SDL_SetTextureColorMod(texture, 0xFF, 0xFF, 0xFF);
    //#if SDL_VERSION_ATLEAST(2, 0, 6)
    //    SDL_BlendMode mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ZERO,
    //                                                    SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ZERO,
    //                                                    SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDOPERATION_ADD);
    //    SDL_SetTextureBlendMode(texture, mode);
    //    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
    //#else
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
    //#endif

    SDL_Rect coords_rect;
    lv_area_to_sdl_rect(&tex_coords, &coords_rect);
    coords_rect.x = 0 - coords_rect.h / 2;
    coords_rect.y = 0 - coords_rect.h / 2;
    SDL_Point center = {coords_rect.h / 2, coords_rect.h / 2};
    SDL_RenderCopyEx(renderer, texture, NULL, &coords_rect, angle, &center, SDL_FLIP_NONE);
    //
    //    /* Draw composited part on screen */
    SDL_SetTextureBlendMode(content, SDL_BLENDMODE_NONE);
    SDL_SetTextureAlphaMod(content, dsc->opa);
    SDL_SetTextureColorMod(content, line_color.r, line_color.g, line_color.b);
    //
    SDL_SetRenderTarget(renderer, screen);
    //    SDL_RenderSetClipRect(renderer, &clip_rect);
    SDL_RenderSetClipRect(renderer, NULL);
    SDL_RenderCopy(renderer, content, &mask_rect, &clip_rect);
    //    SDL_DestroyTexture(mask);
}

static lv_draw_line_key_t line_key_create(lv_coord_t length, lv_coord_t thickness)
{
    lv_draw_line_key_t key;
    /* VERY IMPORTANT! Padding between members is uninitialized, so we have to wipe them manually */
    SDL_memset(&key, 0, sizeof(key));
    key.magic = LV_GPU_CACHE_KEY_MAGIC_LINE;
    key.length = length;
    key.thickness = thickness;
    return key;
}


#endif /*LV_USE_GPU_SDL*/
