/**
 * @file lv_draw_sdl_blend.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "lv_draw_sdl_texture_cache.h"
#include "lv_draw_sdl_utils.h"
#include "lv_draw_sdl_composite.h"

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

static void blend_fill(lv_draw_sdl_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc);

static void blend_map(lv_draw_sdl_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sdl_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    if(dsc->src_buf) {
        blend_map((lv_draw_sdl_ctx_t *) draw_ctx, dsc);
    }
    else {
        blend_fill((lv_draw_sdl_ctx_t *) draw_ctx, dsc);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

void blend_fill(lv_draw_sdl_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    const lv_opa_t opa = dsc->opa;
    const lv_color_t color = dsc->color;
    /*Do not draw transparent things*/
    if(opa < LV_OPA_MIN) return;

    SDL_Renderer * renderer = draw_ctx->renderer;

    lv_area_t fill_area;
    _lv_area_intersect(&fill_area, draw_ctx->base_draw.base_draw.clip_area, dsc->blend_area);
    SDL_Rect fill_rect;
    lv_area_to_sdl_rect(&fill_area, &fill_rect);

    if(dsc->mask) {
        SDL_Texture * texture = lv_draw_sdl_composite_texture_obtain(draw_ctx, LV_DRAW_SDL_COMPOSITE_TEXTURE_ID_STREAM0,
                                                                     lv_area_get_height(&fill_area),
                                                                     lv_area_get_width(&fill_area));
        SDL_Rect rect = {0, 0, lv_area_get_width(&fill_area), lv_area_get_height(&fill_area)};
        uint8_t * pixels = NULL;
        int pitch;
        SDL_LockTexture(texture, &rect, (void **) &pixels, &pitch);
        SDL_assert(pixels && pitch);
        for(int y = 0; y < rect.h; y++) {
            SDL_memset(&pixels[y * pitch], 0xFF, 4 * rect.w);
            for(int x = 0; x < rect.w; x++) {
                pixels[y * pitch + x * 4] = dsc->mask[y * rect.w + x];
            }
        }
        SDL_UnlockTexture(texture);

        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(texture, opa);
        SDL_SetTextureColorMod(texture, color.ch.red, color.ch.green, color.ch.blue);
        SDL_RenderCopy(renderer, texture, &rect, &fill_rect);
    }
    else {
        SDL_SetRenderDrawColor(renderer, color.ch.red, color.ch.green, color.ch.blue, opa);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(renderer, &fill_rect);
    }
}

void blend_map(lv_draw_sdl_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    const lv_opa_t opa = dsc->opa;
    /*Do not draw transparent things*/
    if(opa < LV_OPA_MIN) return;
    lv_coord_t sw = lv_area_get_width(dsc->blend_area), sh = lv_area_get_height(dsc->blend_area);

    SDL_Renderer * renderer = draw_ctx->renderer;

    SDL_Rect draw_area_rect;
    lv_area_to_sdl_rect(draw_ctx->base_draw.base_draw.clip_area, &draw_area_rect);

    Uint32 rmask = 0x00FF0000;
    Uint32 gmask = 0x0000FF00;
    Uint32 bmask = 0x000000FF;
    Uint32 amask = 0x00000000;
    SDL_Surface * surface = SDL_CreateRGBSurfaceFrom((void *) dsc->src_buf, sw, sh, LV_COLOR_DEPTH,
                                                     sw * LV_COLOR_DEPTH / 8, rmask, gmask, bmask, amask);
    if(dsc->mask) {
        SDL_Texture * masked = SDL_CreateTexture(renderer, LV_DRAW_SDL_TEXTURE_FORMAT, SDL_TEXTUREACCESS_TARGET,
                                                 sw, sh);
        SDL_Texture * mask_texture = lv_sdl_create_opa_texture(renderer, dsc->mask, sw, sh, sw);
        SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_SetRenderTarget(renderer, masked);

        SDL_SetTextureAlphaMod(mask_texture, opa);
        SDL_SetTextureBlendMode(mask_texture, SDL_BLENDMODE_NONE);
        SDL_RenderCopy(renderer, mask_texture, NULL, NULL);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_MOD);
        SDL_RenderCopy(renderer, texture, NULL, NULL);

        SDL_SetRenderTarget(renderer, draw_ctx->base_draw.base_draw.buf);
        SDL_SetTextureBlendMode(masked, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(masked, 0xFF);
        SDL_SetTextureColorMod(masked, 0xFF, 0xFF, 0xFF);
        SDL_RenderCopy(renderer, masked, NULL, &draw_area_rect);
        SDL_DestroyTexture(texture);
        SDL_DestroyTexture(mask_texture);
        SDL_DestroyTexture(masked);
    }
    else {
        SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_SetTextureAlphaMod(texture, opa);
        SDL_SetRenderTarget(renderer, draw_ctx->base_draw.base_draw.buf);
        SDL_RenderCopy(renderer, texture, NULL, &draw_area_rect);
        SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
}

#endif /*LV_USE_GPU_SDL*/
