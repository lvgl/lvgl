/**
 * @file lv_draw_m2d_fill.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_M2D
#include "lv_draw_m2d_private.h"

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_draw_m2d_fill_is_supported(const lv_draw_task_t * task, const lv_draw_fill_dsc_t * draw_dsc)
{
    LV_UNUSED(task);

    if((draw_dsc->radius != 0 || draw_dsc->grad.dir != LV_GRAD_DIR_NONE))
        return false;

    return true;
}

void lv_draw_m2d_fill(lv_draw_task_t * t, const lv_draw_fill_dsc_t * draw_dsc, const lv_area_t * coords)
{
    if(draw_dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    lv_layer_t * layer = t->target_layer;
    lv_draw_m2d_buf_t * target = (lv_draw_m2d_buf_t *)layer->draw_buf;

    lv_draw_m2d_buf_sync_for_gpu(target);

    struct m2d_rectangle rect;
    rect.x = coords->x1;
    rect.y = coords->y1;
    rect.w = lv_area_get_width(coords);
    rect.h = lv_area_get_height(coords);

    uint8_t alpha = (draw_dsc->opa >= (lv_opa_t)LV_OPA_MAX) ? 255 : draw_dsc->opa;
    m2d_source_color(draw_dsc->color.red,
                     draw_dsc->color.green,
                     draw_dsc->color.blue,
                     alpha);

    m2d_source_enable(M2D_SRC, false);
    m2d_source_enable(M2D_DST, false);
    m2d_blend_enable(alpha != 255);
    m2d_blend_factors(M2D_BLEND_SRC_ALPHA, M2D_BLEND_ONE_MINUS_SRC_ALPHA,
                      M2D_BLEND_ONE, M2D_BLEND_ONE_MINUS_SRC_ALPHA);
    m2d_set_target(target->m2d_buf);
    m2d_draw_rectangles(&rect, 1);
}

#endif /*LV_USE_DRAW_M2D*/
