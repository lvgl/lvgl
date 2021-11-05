/**
 * @file lv_gpu_sdl_draw_blend.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "../../draw/lv_draw_blend.h"
#include "../../core/lv_refr.h"
#include "lv_gpu_sdl_texture_cache.h"
#include "lv_gpu_sdl_utils.h"
#include "lv_gpu_sdl_mask.h"

#include LV_GPU_SDL_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void _lv_blend_fill(const lv_area_t * clip_area, const lv_area_t * fill_area, lv_color_t color,
                    lv_opa_t * mask, lv_draw_mask_res_t mask_res, lv_opa_t opa, lv_blend_mode_t mode)
{
    /*Do not draw transparent things*/
    if(opa < LV_OPA_MIN) return;
    if(mask_res == LV_DRAW_MASK_RES_TRANSP) return;

    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    SDL_Renderer * renderer = (SDL_Renderer *) disp->driver->user_data;

    /*Get clipped fill area which is the real draw area.
     *It is always the same or inside `fill_area`*/
    lv_area_t draw_area;
    if(!_lv_area_intersect(&draw_area, clip_area, fill_area)) return;

    SDL_Rect draw_area_rect;
    lv_area_to_sdl_rect(&draw_area, &draw_area_rect);
    if(mask) {
        SDL_Surface * mask_surface = lv_sdl_create_mask_surface(mask, lv_area_get_width(&draw_area),
                                                                lv_area_get_height(&draw_area),
                                                                lv_area_get_width(&draw_area));
        SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, mask_surface);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(texture, opa);
        SDL_SetTextureColorMod(texture, color.ch.red, color.ch.green, color.ch.blue);
        SDL_RenderSetClipRect(renderer, &draw_area_rect);
        SDL_RenderCopy(renderer, texture, NULL, &draw_area_rect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(mask_surface);
    }
    else {
        SDL_SetRenderDrawColor(renderer, color.ch.red, color.ch.green, color.ch.blue, opa);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderSetClipRect(renderer, &draw_area_rect);
        SDL_RenderFillRect(renderer, &draw_area_rect);
    }
}

void _lv_blend_map(const lv_area_t * clip_area, const lv_area_t * map_area,
                   const lv_color_t * map_buf, lv_opa_t * mask, lv_draw_mask_res_t mask_res, lv_opa_t opa,
                   lv_blend_mode_t mode)
{
    LV_UNUSED(clip_area);
    LV_UNUSED(map_area);
    LV_UNUSED(map_buf);
    LV_UNUSED(mask);
    LV_UNUSED(mask_res);
    LV_UNUSED(opa);
    LV_UNUSED(mode);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GPU_SDL*/
