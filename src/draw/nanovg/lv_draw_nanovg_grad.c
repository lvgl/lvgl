/**
 * @file lv_draw_nanovg_grad.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg_private.h"

#if LV_USE_DRAW_NANOVG && LV_USE_VECTOR_GRAPHIC

#include "../../draw/lv_draw_vector_private.h"

#include "lv_nanovg_utils.h"

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

bool lv_nanovg_grad_to_paint(NVGcontext * ctx, const lv_vector_gradient_t * grad, NVGpaint * paint)
{
    LV_PROFILER_DRAW_BEGIN;

    LV_ASSERT_NULL(grad);
    LV_ASSERT_NULL(paint);

    if(grad->stops_count < 2) {
        LV_LOG_WARN("stops_count(%d) should be 2 for gradient", grad->stops_count);
        LV_PROFILER_DRAW_END;
        return false;
    }

    const NVGcolor icol = lv_nanovg_color_convert(grad->stops[0].color, grad->stops[0].opa);
    const NVGcolor ocol = lv_nanovg_color_convert(grad->stops[1].color, grad->stops[1].opa);

    switch(grad->style) {
        case LV_VECTOR_GRADIENT_STYLE_LINEAR:
            *paint = nvgLinearGradient(ctx, grad->x1, grad->y1, grad->x2, grad->y2, icol, ocol);
            break;

        case LV_VECTOR_GRADIENT_STYLE_RADIAL: {
                const float inr = grad->cr * grad->stops[0].frac / 255;
                const float outr = grad->cr * grad->stops[1].frac / 255;
                *paint = nvgRadialGradient(ctx, grad->cx, grad->cy, inr, outr, icol, ocol);
            }
            break;

        default:
            LV_LOG_WARN("Unsupported gradient style: %d", grad->style);
            LV_PROFILER_DRAW_END;
            return false;
    }

    LV_PROFILER_DRAW_END;
    return true;
}

void lv_nanovg_draw_grad(
    NVGcontext * ctx,
    const lv_vector_gradient_t * grad,
    enum NVGwinding winding,
    enum NVGcompositeOperation composite_operation)
{
    LV_PROFILER_DRAW_BEGIN;

    NVGpaint paint;
    if(!lv_nanovg_grad_to_paint(ctx, grad, &paint)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    nvgPathWinding(ctx, winding);
    nvgGlobalCompositeOperation(ctx, composite_operation);
    nvgFillPaint(ctx, paint);
    nvgFill(ctx);

    LV_PROFILER_DRAW_END;
}

void lv_nanovg_draw_grad_helper(
    NVGcontext * ctx,
    const lv_area_t * area,
    const lv_grad_dsc_t * grad_dsc,
    enum NVGwinding winding,
    enum NVGcompositeOperation composite_operation)
{
    LV_ASSERT_NULL(ctx);
    LV_ASSERT_NULL(area);
    LV_ASSERT_NULL(grad_dsc);

    lv_vector_gradient_t grad;
    lv_memzero(&grad, sizeof(grad));

    grad.style = LV_VECTOR_GRADIENT_STYLE_LINEAR;
    grad.stops_count = grad_dsc->stops_count;
    lv_memcpy(grad.stops, grad_dsc->stops, sizeof(lv_grad_stop_t) * grad_dsc->stops_count);

    /*convert to spread mode*/
    switch(grad_dsc->extend) {
        case LV_GRAD_EXTEND_PAD:
            grad.spread = LV_VECTOR_GRADIENT_SPREAD_PAD;
            break;
        case LV_GRAD_EXTEND_REPEAT:
            grad.spread = LV_VECTOR_GRADIENT_SPREAD_REPEAT;
            break;
        case LV_GRAD_EXTEND_REFLECT:
            grad.spread = LV_VECTOR_GRADIENT_SPREAD_REFLECT;
            break;
        default:
            LV_LOG_WARN("Unsupported gradient extend mode: %d", grad_dsc->extend);
            grad.spread = LV_VECTOR_GRADIENT_SPREAD_PAD;
            break;
    }

    switch(grad_dsc->dir) {
        case LV_GRAD_DIR_VER:
            grad.x1 = area->x1;
            grad.y1 = area->y1;
            grad.x2 = area->x1;
            grad.y2 = area->y2 + 1;
            break;

        case LV_GRAD_DIR_HOR:
            grad.x1 = area->x1;
            grad.y1 = area->y1;
            grad.x2 = area->x2 + 1;
            grad.y2 = area->y1;
            break;

        case LV_GRAD_DIR_LINEAR: {
                int32_t w = lv_area_get_width(area);
                int32_t h = lv_area_get_height(area);

                grad.x1 = lv_pct_to_px(grad_dsc->params.linear.start.x, w) + area->x1;
                grad.y1 = lv_pct_to_px(grad_dsc->params.linear.start.y, h) + area->y1;
                grad.x2 = lv_pct_to_px(grad_dsc->params.linear.end.x, w) + area->x1;
                grad.y2 = lv_pct_to_px(grad_dsc->params.linear.end.y, h) + area->y1;
            }
            break;

        case LV_GRAD_DIR_RADIAL: {
                grad.style = LV_VECTOR_GRADIENT_STYLE_RADIAL;
                int32_t w = lv_area_get_width(area);
                int32_t h = lv_area_get_height(area);

                grad.cx = lv_pct_to_px(grad_dsc->params.radial.focal.x, w) + area->x1;
                grad.cy = lv_pct_to_px(grad_dsc->params.radial.focal.y, h) + area->y1;
                int32_t end_extent_x = lv_pct_to_px(grad_dsc->params.radial.end_extent.x, w) + area->x1;
                int32_t end_extent_y = lv_pct_to_px(grad_dsc->params.radial.end_extent.y, h) + area->y1;
                grad.cr = LV_MAX(end_extent_x - grad.cx, end_extent_y - grad.cy);
            }
            break;

        default:
            LV_LOG_WARN("Unsupported gradient direction: %d", grad_dsc->dir);
            return;
    }

    lv_nanovg_draw_grad(ctx, &grad, winding, composite_operation);
}

/**********************
*   STATIC FUNCTIONS
**********************/

#endif /* LV_USE_DRAW_NANOVG */
