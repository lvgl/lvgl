/**
 * @file lv_nanovg_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_nanovg_utils.h"

#if LV_USE_DRAW_NANOVG

#include "../../misc/lv_pending.h"
#include "lv_draw_nanovg_private.h"
#include "lv_nanovg_math.h"
#include <float.h>
#include <math.h>

/*********************
*      DEFINES
*********************/

/* Magic number from https://spencermortensen.com/articles/bezier-circle/ */
#define PATH_ARC_MAGIC 0.55191502449351f

#define SIGN(x) (nvg_math_is_zero(x) ? 0 : ((x) > 0 ? 1 : -1))

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

void lv_nanovg_utils_init(struct _lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);
}

void lv_nanovg_utils_deinit(struct _lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);

    if(u->image_buf) {
        lv_draw_buf_destroy(u->image_buf);
        u->image_buf = NULL;
    }
}

void lv_nanovg_transform(NVGcontext * ctx, const lv_matrix_t * matrix)
{
    LV_ASSERT_NULL(ctx);
    LV_ASSERT_NULL(matrix);
    LV_PROFILER_DRAW_BEGIN;

    nvgTransform(ctx,
                 matrix->m[0][0],
                 matrix->m[1][0],
                 matrix->m[0][1],
                 matrix->m[1][1],
                 matrix->m[0][2],
                 matrix->m[1][2]);

    LV_PROFILER_DRAW_END;
}

void lv_nanovg_set_clip_area(NVGcontext * ctx, const lv_area_t * area)
{
    LV_ASSERT_NULL(ctx);
    LV_ASSERT_NULL(area);
    LV_PROFILER_DRAW_BEGIN;

    nvgScissor(ctx,
               area->x1, area->y1,
               lv_area_get_width(area), lv_area_get_height(area));

    LV_PROFILER_DRAW_END;
}

void lv_nanovg_path_append_rect(NVGcontext * ctx, float x, float y, float w, float h, float r)
{
    LV_ASSERT_NULL(ctx);

    LV_PROFILER_DRAW_BEGIN;

    if(r > 0) {
        const float half_w = w / 2.0f;
        const float half_h = h / 2.0f;

        /*clamping cornerRadius by minimum size*/
        const float r_max = LV_MIN(half_w, half_h);

        nvgRoundedRect(ctx, x, y, w, h, r > r_max ? r_max : r);
        LV_PROFILER_DRAW_END;
        return;
    }

    nvgRect(ctx, x, y, w, h);

    LV_PROFILER_DRAW_END;
}

void lv_nanovg_path_append_area(NVGcontext * ctx, const lv_area_t * area)
{
    LV_ASSERT_NULL(ctx);
    LV_ASSERT_NULL(area);
    LV_PROFILER_DRAW_BEGIN;

    nvgRect(ctx, area->x1, area->y1, lv_area_get_width(area), lv_area_get_height(area));

    LV_PROFILER_DRAW_END;
}

void lv_nanovg_path_append_arc_right_angle(NVGcontext * ctx,
                                           float start_x, float start_y,
                                           float center_x, float center_y,
                                           float end_x, float end_y)
{
    LV_PROFILER_DRAW_BEGIN;
    float dx1 = center_x - start_x;
    float dy1 = center_y - start_y;
    float dx2 = end_x - center_x;
    float dy2 = end_y - center_y;

    float c = SIGN(dx1 * dy2 - dx2 * dy1) * PATH_ARC_MAGIC;

    nvgBezierTo(ctx,
                start_x - c * dy1, start_y + c * dx1,
                end_x - c * dy2, end_y + c * dx2,
                end_x, end_y);
    LV_PROFILER_DRAW_END;
}

void lv_nanovg_path_append_arc(NVGcontext * ctx,
                               float cx, float cy,
                               float radius,
                               float start_angle,
                               float sweep,
                               bool pie)
{
    LV_PROFILER_DRAW_BEGIN;

    if(radius <= 0) {
        LV_PROFILER_DRAW_END;
        return;
    }

    /* just circle */
    if(sweep >= 360.0f || sweep <= -360.0f) {
        nvgCircle(ctx, cx, cy, radius);
        LV_PROFILER_DRAW_END;
        return;
    }

    start_angle = NVG_MATH_RADIANS(start_angle);
    sweep = NVG_MATH_RADIANS(sweep);

    int n_curves = (int)ceil(NVG_MATH_FABSF(sweep / NVG_MATH_HALF_PI));
    float sweep_sign = sweep < 0 ? -1.f : 1.f;
    float fract = fmodf(sweep, NVG_MATH_HALF_PI);
    fract = (nvg_math_is_zero(fract)) ? NVG_MATH_HALF_PI * sweep_sign : fract;

    /* Start from here */
    float start_x = radius * NVG_MATH_COSF(start_angle);
    float start_y = radius * NVG_MATH_SINF(start_angle);

    if(pie) {
        nvgMoveTo(ctx, cx, cy);
        nvgLineTo(ctx, start_x + cx, start_y + cy);
    }

    for(int i = 0; i < n_curves; ++i) {
        float end_angle = start_angle + ((i != n_curves - 1) ? NVG_MATH_HALF_PI * sweep_sign : fract);
        float end_x = radius * NVG_MATH_COSF(end_angle);
        float end_y = radius * NVG_MATH_SINF(end_angle);

        /* variables needed to calculate bezier control points */

        /** get bezier control points using article:
         * (http://itc.ktu.lt/index.php/ITC/article/view/11812/6479)
         */
        float ax = start_x;
        float ay = start_y;
        float bx = end_x;
        float by = end_y;
        float q1 = ax * ax + ay * ay;
        float q2 = ax * bx + ay * by + q1;
        float k2 = (4.0f / 3.0f) * ((NVG_MATH_SQRTF(2 * q1 * q2) - q2) / (ax * by - ay * bx));

        /* Next start point is the current end point */
        start_x = end_x;
        start_y = end_y;

        end_x += cx;
        end_y += cy;

        float ctrl1_x = ax - k2 * ay + cx;
        float ctrl1_y = ay + k2 * ax + cy;
        float ctrl2_x = bx + k2 * by + cx;
        float ctrl2_y = by - k2 * bx + cy;

        nvgBezierTo(ctx, ctrl1_x, ctrl1_y, ctrl2_x, ctrl2_y, end_x, end_y);
        start_angle = end_angle;
    }

    if(pie) {
        nvgClosePath(ctx);
    }

    LV_PROFILER_DRAW_END;
}

void lv_nanovg_fill(NVGcontext * ctx, enum NVGwinding winding, enum NVGcompositeOperation composite_operation,
                    NVGcolor color)
{
    LV_ASSERT_NULL(ctx);
    LV_PROFILER_DRAW_BEGIN;
    nvgPathWinding(ctx, winding);
    nvgGlobalCompositeOperation(ctx, composite_operation);
    nvgFillColor(ctx, color);
    nvgFill(ctx);
    LV_PROFILER_DRAW_END;
}

void lv_nanovg_end_frame(struct _lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);
    LV_PROFILER_DRAW_BEGIN;

    if(!u->is_started) {
        LV_PROFILER_DRAW_END;
        return;
    }

    LV_PROFILER_DRAW_BEGIN_TAG("nvgEndFrame");
    nvgEndFrame(u->vg);
    LV_PROFILER_DRAW_END_TAG("nvgEndFrame");

    lv_nanovg_clean_up(u);

    LV_PROFILER_DRAW_END;
}

void lv_nanovg_clean_up(struct _lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);
    LV_PROFILER_DRAW_BEGIN;

    lv_pending_remove_all(u->image_pending);
    lv_pending_remove_all(u->letter_pending);
    u->is_started = false;

    LV_PROFILER_DRAW_END;
}

lv_draw_buf_t * lv_nanovg_reshape_global_image(struct _lv_draw_nanovg_unit_t * u,
                                               lv_color_format_t cf,
                                               uint32_t w,
                                               uint32_t h)
{
    LV_ASSERT_NULL(u);

    uint32_t stride = (w * lv_color_format_get_bpp(cf) + 7) >> 3;
    lv_draw_buf_t * tmp_buf = lv_draw_buf_reshape(u->image_buf, cf, w, h, stride);

    if(!tmp_buf) {
        if(u->image_buf) {
            lv_draw_buf_destroy(u->image_buf);
            u->image_buf = NULL;
        }

        tmp_buf = lv_draw_buf_create(w, h, cf, stride);
        if(!tmp_buf) {
            return NULL;
        }
    }

    u->image_buf = tmp_buf;

    return u->image_buf;
}

/**********************
*   STATIC FUNCTIONS
**********************/

#endif /* LV_USE_DRAW_NANOVG */
