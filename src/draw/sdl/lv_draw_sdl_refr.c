/**
 * @file lv_draw_sdl_refr.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
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

void lv_draw_sdl_refr_obj_transformed(lv_draw_ctx_t * draw_ctx, lv_obj_t * obj)
{
    lv_opa_t opa = lv_obj_get_style_opa(obj, 0);
    if(opa < LV_OPA_MIN) return;

    lv_draw_sdl_ctx_t * draw_ctx_sdl = (lv_draw_sdl_ctx_t *) draw_ctx;

    /* Calculate full coords first, let's optimize later... */
    lv_area_t draw_area;
    const lv_area_t buf_area_ori = *draw_ctx->buf_area;
    const lv_area_t * clip_area_ori = draw_ctx->clip_area;
    lv_coord_t ext_draw_size = _lv_obj_get_ext_draw_size(obj);
    lv_area_t obj_coords_ext;
    lv_obj_get_coords(obj, &obj_coords_ext);
    lv_area_increase(&obj_coords_ext, ext_draw_size, ext_draw_size);

    draw_area = obj_coords_ext;

    lv_area_t trans_coords_ext = obj_coords_ext;
    lv_obj_get_transformed_area(obj, &trans_coords_ext, false, false);

    lv_area_t trans_area = trans_coords_ext;

    SDL_Rect draw_rect = {0, 0, lv_area_get_width(&draw_area), lv_area_get_height(&draw_area)};
    SDL_Rect trans_rect;
    lv_area_to_sdl_rect(&trans_area, &trans_rect);

    lv_point_t trans_offset = {-draw_area.x1, -draw_area.y1};

    SDL_Renderer * renderer = draw_ctx_sdl->renderer;
    SDL_Texture * target = lv_draw_sdl_composite_texture_obtain(draw_ctx_sdl, LV_DRAW_SDL_COMPOSITE_TEXTURE_ID_TARGET1,
                                                                draw_rect.w, draw_rect.h);
    SDL_SetTextureBlendMode(target, SDL_BLENDMODE_BLEND);

    SDL_Texture * old_target = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, target);
    SDL_RenderClear(renderer);

    /* Set proper drawing context for transform layer */
    draw_ctx_sdl->internals->transform_offset = &trans_offset;
    *draw_ctx->buf_area = draw_area;
    draw_ctx->clip_area = &draw_area;

    /* Draw object on transform layer */
    lv_obj_redraw(draw_ctx, obj);

    /* Reset drawing context */
    *draw_ctx->buf_area = buf_area_ori;
    draw_ctx->clip_area = clip_area_ori;
    draw_ctx_sdl->internals->transform_offset = NULL;

    SDL_SetRenderTarget(renderer, old_target);

    /*Render off-screen texture, transformed*/

    lv_point_t pivot = {
        .x = ext_draw_size + lv_obj_get_style_transform_pivot_x(obj, 0),
        .y = ext_draw_size + lv_obj_get_style_transform_pivot_y(obj, 0)
    };
    lv_obj_transform_point(obj, &pivot, false, false);

    SDL_Rect clip_rect;
    lv_area_to_sdl_rect(clip_area_ori, &clip_rect);
    SDL_Point center = {.x = pivot.x, .y = pivot.y};
    lv_coord_t angle = lv_obj_get_style_transform_angle(obj, 0);
    SDL_RenderSetClipRect(renderer, &clip_rect);
    SDL_RenderCopyEx(renderer, target, &draw_rect, &trans_rect, angle, &center, SDL_FLIP_NONE);
    SDL_RenderSetClipRect(renderer, NULL);
}

void lv_draw_sdl_refr_areas_offset(lv_draw_sdl_ctx_t * ctx, bool has_composite, lv_area_t * apply_area,
                                   lv_area_t * coords, lv_area_t * clip)
{
    const lv_point_t * offset = ctx->internals->transform_offset;
    if(!offset) {
        return;
    }
    if(has_composite) {
        lv_area_move(apply_area, offset->x, offset->y);
    }
    else {
        lv_area_move(coords, offset->x, offset->y);
        lv_area_move(clip, offset->x, offset->y);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
