/**
 * @file lv_draw_sdl_bg.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_SDL

#include "../lv_draw_rect.h"
#include "../lv_draw_img.h"
#include "../lv_draw_label.h"
#include "../lv_draw_mask.h"
#include "../../core/lv_refr.h"
#include "lv_draw_sdl_utils.h"
#include "lv_draw_sdl_texture_cache.h"
#include "lv_draw_sdl_composite.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void draw_bg_color(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords, const lv_area_t * draw_area,
                          const lv_draw_rect_dsc_t * dsc);

static void draw_bg_img(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords, const lv_area_t * draw_area,
                        const lv_draw_rect_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sdl_draw_bg(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    const lv_area_t * clip = draw_ctx->clip_area;
    lv_draw_sdl_ctx_t * ctx = (lv_draw_sdl_ctx_t *) draw_ctx;
    /* Coords will be translated so coords will start at (0,0) */
    lv_area_t t_area;
    bool has_content = _lv_area_intersect(&t_area, coords, clip);

    /* Shadows and outlines will also draw in extended area */
    if(has_content) {
        if(dsc->bg_img_src) {
            draw_bg_img(ctx, coords, &t_area, dsc);
        }
        else {
            draw_bg_color(ctx, coords, &t_area, dsc);
        }
    }

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_bg_color(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords, const lv_area_t * draw_area,
                          const lv_draw_rect_dsc_t * dsc)
{
    SDL_Color bg_color;
    lv_color_to_sdl_color(&dsc->bg_color, &bg_color);
    SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(ctx->renderer, bg_color.r, bg_color.g, bg_color.b, dsc->bg_opa);

    SDL_Rect rect;
    lv_area_to_sdl_rect(draw_area, &rect);
    SDL_RenderFillRect(ctx->renderer, &rect);

    SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);
}

static void draw_bg_img(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords, const lv_area_t * draw_area,
                        const lv_draw_rect_dsc_t * dsc)
{
    SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 0);

    SDL_Rect rect;
    lv_area_to_sdl_rect(draw_area, &rect);
    SDL_RenderFillRect(ctx->renderer, &rect);

    SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);
    lv_draw_rect((lv_draw_ctx_t *) ctx, dsc, coords);
}

#endif /*LV_USE_DRAW_SDL*/
