/**
 * @file lv_draw_sdl_refr.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include "../../core/lv_refr.h"
#include "../../core/lv_disp.h"

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

void lv_draw_sdl_refr_obj_transformed(struct _lv_draw_ctx_t * draw_ctx, struct _lv_obj_t * obj)
{
    lv_draw_sdl_ctx_t * draw_ctx_sdl = (lv_draw_sdl_ctx_t *) draw_ctx;

    const lv_area_t buf_area_backup = *draw_ctx->buf_area;
    const lv_area_t * clip_area_backup = draw_ctx->clip_area;


    lv_coord_t ext_draw_size = _lv_obj_get_ext_draw_size(obj);
    lv_area_t obj_coords_ext;
    lv_obj_get_coords(obj, &obj_coords_ext);
    lv_area_increase(&obj_coords_ext, ext_draw_size, ext_draw_size);

    SDL_Rect srcrect = {0, 0, lv_area_get_width(&obj_coords_ext), lv_area_get_height(&obj_coords_ext)};

    lv_point_t offset = {-obj_coords_ext.x1, -obj_coords_ext.y1};

    SDL_Rect dstrect;

    lv_area_t transf_coords = obj_coords_ext;
    lv_obj_get_transformed_area(obj, &transf_coords, false, false);
    lv_area_to_sdl_rect(&transf_coords, &dstrect);


    SDL_Renderer * renderer = draw_ctx_sdl->renderer;
    SDL_Texture * target = lv_draw_sdl_composite_texture_obtain(draw_ctx_sdl, LV_DRAW_SDL_COMPOSITE_TEXTURE_ID_TARGET1,
                                                                srcrect.w, srcrect.h);

    SDL_Texture * old_target = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, target);
    SDL_RenderClear(renderer);

    draw_ctx_sdl->internals->transform_offset = &offset;

    lv_obj_redraw(draw_ctx, obj);

    draw_ctx_sdl->internals->transform_offset = NULL;

    SDL_SetRenderTarget(renderer, old_target);

    SDL_Rect clip_rect;
    lv_area_to_sdl_rect(clip_area_backup, &clip_rect);
    SDL_RenderSetClipRect(renderer, &clip_rect);
    SDL_RenderCopy(renderer, target, &srcrect, &dstrect);
    SDL_RenderSetClipRect(renderer, NULL);

    SDL_SetRenderDrawColor(renderer, lv_rand(128, 255), lv_rand(128, 255), lv_rand(128, 255), 255);
    SDL_Rect highlight_rect;
    lv_area_to_sdl_rect(&buf_area_backup, &clip_rect);
    SDL_RenderDrawRect(renderer, &highlight_rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
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
