/**
 * @file lv_draw_sdl_refr.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "../../core/lv_refr.h"

#include "lv_draw_sdl.h"
#include "lv_draw_sdl_priv.h"
#include "lv_draw_sdl_composite.h"
#include "lv_draw_sdl_utils.h"
#include "lv_draw_sdl_layer.h"

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

lv_draw_layer_ctx_t * lv_draw_sdl_layer_init(lv_draw_ctx_t * draw_ctx, lv_draw_layer_ctx_t * layer_ctx,
                                             lv_draw_layer_flags_t flags)
{
    lv_draw_sdl_ctx_t * ctx = (lv_draw_sdl_ctx_t *) draw_ctx;
    SDL_Renderer * renderer = ctx->renderer;

    lv_draw_sdl_layer_ctx_t * transform_ctx = (lv_draw_sdl_layer_ctx_t *) layer_ctx;

    transform_ctx->flags = flags;
    transform_ctx->orig_target = SDL_GetRenderTarget(renderer);

    lv_coord_t target_w = lv_area_get_width(&layer_ctx->area_full);
    lv_coord_t target_h = lv_area_get_height(&layer_ctx->area_full);

    enum lv_draw_sdl_composite_texture_id_t texture_id = LV_DRAW_SDL_COMPOSITE_TEXTURE_ID_TRANSFORM0 +
                                                         ctx->internals->transform_count;
    transform_ctx->target = lv_draw_sdl_composite_texture_obtain(ctx, texture_id, target_w, target_h);
    transform_ctx->target_rect.x = 0;
    transform_ctx->target_rect.y = 0;
    transform_ctx->target_rect.w = target_w;
    transform_ctx->target_rect.h = target_h;

    SDL_SetTextureBlendMode(transform_ctx->target, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, transform_ctx->target);
    SDL_RenderClear(renderer);

    /* Set proper drawing context for transform layer */
    ctx->internals->transform_count += 1;
    draw_ctx->buf_area = &layer_ctx->area_full;
    draw_ctx->clip_area = &layer_ctx->area_full;

    return layer_ctx;
}

void lv_draw_sdl_layer_blend(lv_draw_ctx_t * draw_ctx, lv_draw_layer_ctx_t * layer_ctx,
                             const lv_draw_img_dsc_t * draw_dsc)
{
    lv_draw_sdl_ctx_t * ctx = (lv_draw_sdl_ctx_t *) draw_ctx;
    lv_draw_sdl_layer_ctx_t * transform_ctx = (lv_draw_sdl_layer_ctx_t *) layer_ctx;

    SDL_Renderer * renderer = ctx->renderer;

    SDL_Rect trans_rect;

    if(transform_ctx->flags & LV_DRAW_LAYER_FLAG_CAN_SUBDIVIDE) {
        lv_area_zoom_to_sdl_rect(&layer_ctx->area_act, &trans_rect, draw_dsc->zoom, &draw_dsc->pivot);
    }
    else {
        lv_area_zoom_to_sdl_rect(&layer_ctx->area_full, &trans_rect, draw_dsc->zoom, &draw_dsc->pivot);
    }

    SDL_SetRenderTarget(renderer, transform_ctx->orig_target);

    /*Render off-screen texture, transformed*/
    SDL_Rect clip_rect;
    lv_area_to_sdl_rect(layer_ctx->original.clip_area, &clip_rect);
    SDL_Point center = {.x = draw_dsc->pivot.x, .y = draw_dsc->pivot.y};
    SDL_RenderSetClipRect(renderer, &clip_rect);
    SDL_RenderCopyEx(renderer, transform_ctx->target, &transform_ctx->target_rect, &trans_rect,
                     draw_dsc->angle, &center, SDL_FLIP_NONE);
    SDL_RenderSetClipRect(renderer, NULL);
}

void lv_draw_sdl_layer_destroy(lv_draw_ctx_t * draw_ctx, lv_draw_layer_ctx_t * layer_ctx)
{
    lv_draw_sdl_ctx_t * ctx = (lv_draw_sdl_ctx_t *) draw_ctx;
    ctx->internals->transform_count -= 1;
}

void lv_draw_sdl_transform_areas_offset(lv_draw_sdl_ctx_t * ctx, bool has_composite, lv_area_t * apply_area,
                                        lv_area_t * coords, lv_area_t * clip)
{
    if(ctx->internals->transform_count == 0) {
        return;
    }
    lv_area_t * area = ctx->base_draw.buf_area;
    lv_area_move(coords, -area->x1, -area->y1);
    lv_area_move(clip, -area->x1, -area->y1);
    if(has_composite) {
        lv_area_move(apply_area, -area->x1, -area->y1);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GPU_SDL*/
