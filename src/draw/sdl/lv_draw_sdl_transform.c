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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_draw_layer_ctx_t {
    lv_area_t buf_area_ori;
    const lv_area_t * clip_area_ori;
    SDL_Texture * render_target_ori;

    SDL_Texture * transform_target;
    SDL_Rect target_rect;
};
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

lv_draw_layer_ctx_t * lv_draw_sdl_create_layer(lv_draw_ctx_t * draw_ctx, const lv_area_t * src_area)
{
    lv_draw_sdl_ctx_t * ctx = (lv_draw_sdl_ctx_t *) draw_ctx;
    SDL_Renderer * renderer = ctx->renderer;

    lv_draw_layer_ctx_t * transform_ctx = lv_mem_alloc(sizeof(lv_draw_layer_ctx_t));

    transform_ctx->buf_area_ori = *draw_ctx->buf_area;
    transform_ctx->clip_area_ori = draw_ctx->clip_area;
    transform_ctx->render_target_ori = SDL_GetRenderTarget(renderer);

    lv_coord_t target_w = lv_area_get_width(src_area);
    lv_coord_t target_h = lv_area_get_height(src_area);

    enum lv_draw_sdl_composite_texture_id_t texture_id = LV_DRAW_SDL_COMPOSITE_TEXTURE_ID_TRANSFORM0 +
                                                         ctx->internals->transform_count;
    transform_ctx->transform_target = lv_draw_sdl_composite_texture_obtain(ctx, texture_id, target_w, target_h);
    transform_ctx->target_rect.x = 0;
    transform_ctx->target_rect.y = 0;
    transform_ctx->target_rect.w = target_w;
    transform_ctx->target_rect.h = target_h;

    SDL_SetTextureBlendMode(transform_ctx->transform_target, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, transform_ctx->transform_target);
    SDL_RenderClear(renderer);

    /* Set proper drawing context for transform layer */
    ctx->internals->transform_count += 1;
    *draw_ctx->buf_area = *src_area;
    draw_ctx->clip_area = src_area;

    return transform_ctx;
}

void lv_draw_sdl_blend_layer(lv_draw_ctx_t * draw_ctx, lv_draw_layer_ctx_t * transform_ctx,
                             const lv_area_t * trans_area, const lv_point_t * trans_pivot, lv_coord_t trans_angle)
{
    lv_draw_sdl_ctx_t * ctx = (lv_draw_sdl_ctx_t *) draw_ctx;
    ctx->internals->transform_count -= 1;

    SDL_Renderer * renderer = ctx->renderer;

    /* Restore drawing context */
    *draw_ctx->buf_area = transform_ctx->buf_area_ori;
    draw_ctx->clip_area = transform_ctx->clip_area_ori;
    SDL_Rect trans_rect;
    lv_area_to_sdl_rect(trans_area, &trans_rect);

    SDL_SetRenderTarget(renderer, transform_ctx->render_target_ori);

    /*Render off-screen texture, transformed*/

    SDL_Rect clip_rect;
    lv_area_to_sdl_rect(transform_ctx->clip_area_ori, &clip_rect);
    SDL_Point center = {.x = trans_pivot->x, .y = trans_pivot->y};
    SDL_RenderSetClipRect(renderer, &clip_rect);
    SDL_RenderCopyEx(renderer, transform_ctx->transform_target, &transform_ctx->target_rect, &trans_rect,
                     trans_angle, &center, SDL_FLIP_NONE);
    SDL_RenderSetClipRect(renderer, NULL);

    lv_mem_free(transform_ctx);
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
