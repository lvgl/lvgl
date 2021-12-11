/**
 * @file lv_draw_sdl_draw_blend.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_SDL

#include "../lv_draw_blend.h"
#include "lv_draw_sdl_texture_cache.h"
#include "lv_draw_sdl_utils.h"
#include "lv_draw_sdl_mask.h"

#include LV_DRAW_SDL_INCLUDE_PATH

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

void lv_draw_sdl_draw_blend_fill(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * fill_area,
                                 lv_color_t color, lv_opa_t * mask, lv_opa_t opa, lv_blend_mode_t blend_mode)
{
    LV_UNUSED(dest_buf);
    LV_UNUSED(dest_stride);
    /*Do not draw transparent things*/
    if(opa < LV_OPA_MIN) return;

    lv_draw_sdl_context_t * ctx = lv_draw_sdl_get_context();
    SDL_Renderer * renderer = ctx->renderer;

    SDL_Rect fill_rect;
    lv_area_to_sdl_rect(fill_area, &fill_rect);

    if(mask) {
        SDL_Texture * texture = lv_draw_sdl_mask_tmp_obtain(ctx, LV_DRAW_SDL_MASK_KEY_ID_MASK,
                                                            lv_area_get_height(fill_area),
                                                            lv_area_get_width(fill_area));
        SDL_Rect rect = {0, 0, lv_area_get_width(fill_area), lv_area_get_height(fill_area)};
        uint8_t * pixels = NULL;
        int pitch;
        SDL_LockTexture(texture, &rect, (void **) &pixels, &pitch);
        SDL_assert(pixels && pitch);
        for(int y = 0; y < rect.h; y++) {
            SDL_memset(&pixels[y * pitch], 0xFF, 4 * rect.w);
            for(int x = 0; x < rect.w; x++) {
                pixels[y * pitch + x * 4] = mask[y * rect.w + x];
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

void lv_draw_sdl_draw_blend_map(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * clip_area,
                                const lv_color_t * src_buf, const lv_area_t * src_area,
                                lv_opa_t * mask, lv_opa_t opa, lv_blend_mode_t blend_mode)
{
    LV_UNUSED(dest_buf);
    LV_UNUSED(dest_stride);
    /*Do not draw transparent things*/
    if(opa < LV_OPA_MIN) return;

    lv_draw_sdl_context_t * ctx = lv_draw_sdl_get_context();
    SDL_Renderer * renderer = ctx->renderer;

    SDL_Rect draw_area_rect;
    lv_area_to_sdl_rect(clip_area, &draw_area_rect);

    Uint32 rmask = 0x00FF0000;
    Uint32 gmask = 0x0000FF00;
    Uint32 bmask = 0x000000FF;
    Uint32 amask = 0x00000000;
    SDL_Surface * surface = SDL_CreateRGBSurfaceFrom((void *) src_buf, lv_area_get_width(src_area),
                                                     lv_area_get_height(src_area), LV_COLOR_DEPTH,
                                                     lv_area_get_width(src_area) * LV_COLOR_DEPTH / 8,
                                                     rmask, gmask, bmask, amask);
    if(mask) {
        SDL_Texture * masked = SDL_CreateTexture(renderer, LV_DRAW_SDL_TEXTURE_FORMAT, SDL_TEXTUREACCESS_TARGET,
                                                 lv_area_get_width(src_area), lv_area_get_height(src_area));
        SDL_Texture * mask_texture = lv_sdl_create_opa_texture(renderer, mask, lv_area_get_width(src_area),
                                                               lv_area_get_height(src_area),
                                                               lv_area_get_width(src_area));
        SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_SetRenderTarget(renderer, masked);

        SDL_SetTextureAlphaMod(mask_texture, opa);
        SDL_SetTextureBlendMode(mask_texture, SDL_BLENDMODE_NONE);
        SDL_RenderCopy(renderer, mask_texture, NULL, NULL);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_MOD);
        SDL_RenderCopy(renderer, texture, NULL, NULL);

        SDL_SetRenderTarget(renderer, ctx->texture);
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
        SDL_SetRenderTarget(renderer, ctx->texture);
        SDL_RenderCopy(renderer, texture, NULL, &draw_area_rect);
        SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_SDL*/
