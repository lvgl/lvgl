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
#include "../../hal/lv_hal_disp.h"
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

void lv_gpu_sdl_draw_blend_fill(lv_color_t *dest_buf, lv_coord_t dest_stride, const lv_area_t *fill_area,
                                lv_color_t color, lv_opa_t *mask, lv_opa_t opa, lv_blend_mode_t blend_mode) {
    LV_UNUSED(dest_buf);
    /*Do not draw transparent things*/
    if (opa < LV_OPA_MIN) return;

    lv_gpu_sdl_backend_context_t *ctx = lv_gpu_sdl_get_context();
    SDL_Renderer *renderer = ctx->renderer;

    lv_disp_t *disp = _lv_refr_get_disp_refreshing();
    lv_disp_draw_buf_t *draw_buf = lv_disp_get_draw_buf(disp);

    /*Get clipped fill area which is the real draw area.
     *It is always the same or inside `fill_area`*/
    lv_area_t draw_area = *fill_area;
//    if(!_lv_area_intersect(&draw_area, clip_area, fill_area)) return;

    SDL_Rect draw_area_rect;
    lv_area_to_sdl_rect(&draw_area, &draw_area_rect);

    // TODO: this function needs absolute coordinate
    draw_area_rect.x += draw_buf->area.x1;
    draw_area_rect.y += draw_buf->area.y1;

    if (mask) {
        SDL_Surface *mask_surface = lv_sdl_create_mask_surface(mask, lv_area_get_width(&draw_area),
                                                               lv_area_get_height(&draw_area),
                                                               lv_area_get_width(&draw_area));
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, mask_surface);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(texture, opa);
        SDL_SetTextureColorMod(texture, color.ch.red, color.ch.green, color.ch.blue);
        SDL_RenderSetClipRect(renderer, &draw_area_rect);
        SDL_RenderCopy(renderer, texture, NULL, &draw_area_rect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(mask_surface);
    } else {
        SDL_SetRenderDrawColor(renderer, color.ch.red, color.ch.green, color.ch.blue, opa);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderSetClipRect(renderer, &draw_area_rect);
        SDL_RenderFillRect(renderer, &draw_area_rect);
    }
}

void lv_gpu_sdl_draw_blend_map(lv_color_t *dest_buf, lv_coord_t dest_stride, const lv_area_t *clip_area,
                               const lv_color_t *src_buf, const lv_area_t *src_area,
                               lv_opa_t *mask, lv_opa_t opa, lv_blend_mode_t blend_mode) {
    LV_UNUSED(dest_buf);
    /*Do not draw transparent things*/
    if (opa < LV_OPA_MIN) return;

    lv_gpu_sdl_backend_context_t *ctx = lv_gpu_sdl_get_context();
    SDL_Renderer *renderer = ctx->renderer;

    lv_disp_t *disp = _lv_refr_get_disp_refreshing();
    lv_disp_draw_buf_t *draw_buf = lv_disp_get_draw_buf(disp);

    SDL_Rect draw_area_rect;
    lv_area_to_sdl_rect(clip_area, &draw_area_rect);

    // TODO: this function needs absolute coordinate
    draw_area_rect.x += draw_buf->area.x1;
    draw_area_rect.y += draw_buf->area.y1;

    Uint32 rmask = 0x00FF0000;
    Uint32 gmask = 0x0000FF00;
    Uint32 bmask = 0x000000FF;
    Uint32 amask = 0x00000000;
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void *) src_buf, lv_area_get_width(src_area),
                                                    lv_area_get_height(src_area), LV_COLOR_DEPTH,
                                                    lv_area_get_width(src_area) * LV_COLOR_DEPTH / 8,
                                                    rmask, gmask, bmask, amask);
    if (mask) {
        SDL_Texture *masked = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                                                lv_area_get_width(src_area), lv_area_get_height(src_area));
        SDL_Texture *mask_texture = lv_sdl_create_mask_texture(renderer, mask, lv_area_get_width(src_area),
                                                               lv_area_get_height(src_area),
                                                               lv_area_get_width(src_area));
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_SetRenderTarget(renderer, masked);

        SDL_RenderSetClipRect(renderer, NULL);
        SDL_SetTextureAlphaMod(mask_texture, opa);
        SDL_SetTextureBlendMode(mask_texture, SDL_BLENDMODE_NONE);
        SDL_RenderCopy(renderer, mask_texture, NULL, NULL);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_MOD);
        SDL_RenderCopy(renderer, texture, NULL, NULL);

        SDL_SetRenderTarget(renderer, ctx->texture);
        SDL_RenderSetClipRect(renderer, &draw_area_rect);
        SDL_SetTextureBlendMode(masked, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(masked, 0xFF);
        SDL_SetTextureColorMod(masked, 0xFF, 0xFF, 0xFF);
        SDL_RenderCopy(renderer, masked, NULL, &draw_area_rect);
        SDL_DestroyTexture(texture);
        SDL_DestroyTexture(mask_texture);
        SDL_DestroyTexture(masked);
    } else {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_SetTextureAlphaMod(texture, opa);
        SDL_SetRenderTarget(renderer, ctx->texture);
        SDL_RenderSetClipRect(renderer, &draw_area_rect);
        SDL_RenderCopy(renderer, texture, NULL, &draw_area_rect);
        SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GPU_SDL*/
