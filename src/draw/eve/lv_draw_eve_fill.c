/**
 * @file lv_draw_eve_fill.c
 *
 */

/*  Created on: 27 mar 2023
 *      Author: juanj
 *
 *  Modified by LVGL
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_eve_private.h"
#if LV_USE_DRAW_EVE
#include "lv_eve.h"


/**********************
 * STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_eve_fill(lv_draw_task_t * t, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{

    int32_t rad = dsc->radius;
    int32_t bg_w = lv_area_get_width(coords);
    int32_t bg_h = lv_area_get_height(coords);
    int32_t real_radius = LV_MIN3(bg_w / 2, bg_h / 2, rad);

    lv_eve_scissor(t->clip_area.x1, t->clip_area.y1, t->clip_area.x2, t->clip_area.y2);
    lv_eve_save_context();

    lv_eve_color(dsc->color);
    lv_eve_color_opa(dsc->opa);

    if(bg_w == bg_h && rad == LV_RADIUS_CIRCLE) {
        lv_eve_draw_circle_simple(coords->x1 + (bg_w / 2), coords->y1 + (bg_h / 2), real_radius);
    }
    else {
        lv_eve_draw_rect_simple(coords->x1, coords->y1, coords->x2, coords->y2, real_radius);
    }

    lv_eve_restore_context();
}


/**********************
 *   STATIC FUNCTIONS
 **********************/



void lv_draw_eve_border(lv_draw_task_t * t, const lv_draw_border_dsc_t * dsc, const lv_area_t * coords)
{

    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width == 0) return;
    if(dsc->side == LV_BORDER_SIDE_NONE) return;

    int32_t coords_w = lv_area_get_width(coords);
    int32_t coords_h = lv_area_get_height(coords);
    int32_t rout = dsc->radius;
    int32_t short_side = LV_MIN(coords_w, coords_h);
    if(rout > short_side >> 1) rout = short_side >> 1;

    /*Get the inner area*/
    lv_area_t area_inner;
    lv_area_copy(&area_inner, coords);
    area_inner.x1 += ((dsc->side & LV_BORDER_SIDE_LEFT) ? dsc->width : - (dsc->width));
    area_inner.x2 -= ((dsc->side & LV_BORDER_SIDE_RIGHT) ? dsc->width : - (dsc->width));
    area_inner.y1 += ((dsc->side & LV_BORDER_SIDE_TOP) ? dsc->width : - (dsc->width));
    area_inner.y2 -= ((dsc->side & LV_BORDER_SIDE_BOTTOM) ? dsc->width : - (dsc->width));

    int32_t rin = rout - dsc->width;
    if(rin < 0) rin = 0;

    lv_eve_save_context();

    lv_eve_scissor(t->clip_area.x1, t->clip_area.y1, t->clip_area.x2, t->clip_area.y2);

    lv_eve_color(dsc->color);
    lv_eve_color_opa(dsc->opa);

    lv_eve_color_mask(0, 0, 0, 1);
    lv_eve_stencil_func(EVE_ALWAYS, 0, 1);
    lv_eve_stencil_op(EVE_REPLACE, EVE_REPLACE);
    lv_eve_draw_rect_simple(coords->x1, coords->y1, coords->x2, coords->y2, 0);

    lv_eve_blend_func(EVE_ONE, EVE_ZERO);
    lv_eve_draw_rect_simple(area_inner.x1 - 2, area_inner.y1 - 1, area_inner.x2 + 1, area_inner.y2 + 2, rin);

    lv_eve_stencil_func(EVE_ALWAYS, 1, 1);
    lv_eve_stencil_op(EVE_REPLACE, EVE_REPLACE);
    lv_eve_blend_func(EVE_ZERO, EVE_ONE_MINUS_SRC_ALPHA);
    lv_eve_color_opa(255);
    lv_eve_draw_rect_simple(area_inner.x1, area_inner.y1, area_inner.x2, area_inner.y2, rin);

    lv_eve_color_mask(1, 1, 1, 1);

    if(dsc->side == LV_BORDER_SIDE_FULL) {
        lv_eve_blend_func(EVE_DST_ALPHA, EVE_ONE_MINUS_DST_ALPHA);
        lv_eve_draw_rect_simple(area_inner.x1, area_inner.y1, area_inner.x2, area_inner.y2, rin);
    }

    lv_eve_stencil_func(EVE_NOTEQUAL, 1, 255);
    lv_eve_blend_func(EVE_SRC_ALPHA, EVE_ONE_MINUS_SRC_ALPHA);

    lv_eve_color_opa(dsc->opa);
    lv_eve_draw_rect_simple(coords->x1, coords->y1, coords->x2, coords->y2, rout);

    lv_eve_restore_context();
}

#endif /*LV_USE_DRAW_EVE*/

