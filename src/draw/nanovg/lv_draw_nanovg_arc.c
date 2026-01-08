/**
 * @file lv_draw_nanovg_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg_private.h"

#if LV_USE_DRAW_NANOVG

#include "lv_nanovg_math.h"
#include "lv_nanovg_utils.h"
#include "lv_nanovg_image_cache.h"

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

void lv_draw_nanovg_arc(lv_draw_task_t * t, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords)
{
    LV_PROFILER_DRAW_BEGIN;

    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)t->draw_unit;

    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, coords, &t->clip_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    float start_angle = dsc->start_angle;
    float end_angle = dsc->end_angle;
    float sweep_angle = end_angle - start_angle;

    while(sweep_angle < 0) {
        sweep_angle += 360;
    }

    while(sweep_angle > 360) {
        sweep_angle -= 360;
    }

    /*If the angles are the same then there is nothing to draw*/
    if(nvg_math_is_zero(sweep_angle)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    nvgBeginPath(u->vg);

    float radius_out = dsc->radius;
    float radius_in = dsc->radius - dsc->width;
    float cx = dsc->center.x;
    float cy = dsc->center.y;

    enum NVGwinding winding = NVG_CCW;

    if(nvg_math_is_equal(sweep_angle, 360)) {
        nvgCircle(u->vg, cx, cy, radius_out);

        /* radius_in <= 0, normal fill circle */
        if(radius_in > 0) {
            nvgCircle(u->vg, cx, cy, radius_in);
        }
        winding = NVG_CW;
    }
    else {
        float start_angle_rad = NVG_MATH_RADIANS(start_angle);
        float end_angle_rad = NVG_MATH_RADIANS(end_angle);

        if(radius_in > 0) {
            /* radius_out start point */
            float start_x = radius_out * NVG_MATH_COSF(start_angle_rad) + cx;
            float start_y = radius_out * NVG_MATH_SINF(start_angle_rad) + cy;

            /* radius_in start point */
            float end_x = radius_in * NVG_MATH_COSF(end_angle_rad) + cx;
            float end_y = radius_in * NVG_MATH_SINF(end_angle_rad) + cy;

            nvgMoveTo(u->vg, start_x, start_y);

            /* radius_out arc */
            lv_nanovg_path_append_arc(u->vg,
                                      cx, cy,
                                      radius_out,
                                      start_angle,
                                      sweep_angle,
                                      false);

            /* line to radius_in */
            nvgLineTo(u->vg, end_x, end_y);

            /* radius_in arc */
            lv_nanovg_path_append_arc(u->vg,
                                      cx, cy,
                                      radius_in,
                                      end_angle,
                                      -sweep_angle,
                                      false);

            /* close arc */
            nvgClosePath(u->vg);
        }
        else {
            /* draw a normal arc pie shape */
            lv_nanovg_path_append_arc(u->vg, cx, cy, radius_out, start_angle, sweep_angle, true);
        }

        /* draw round */
        if(dsc->rounded && dsc->width > 0) {
            float round_radius = radius_out > dsc->width ? dsc->width / 2.0f : radius_out / 2.0f;
            float round_center = radius_out - round_radius;
            float rcx1 = cx + round_center * NVG_MATH_COSF(end_angle_rad);
            float rcy1 = cy + round_center * NVG_MATH_SINF(end_angle_rad);
            nvgCircle(u->vg, rcx1, rcy1, round_radius);

            float rcx2 = cx + round_center * NVG_MATH_COSF(start_angle_rad);
            float rcy2 = cy + round_center * NVG_MATH_SINF(start_angle_rad);
            nvgCircle(u->vg, rcx2, rcy2, round_radius);
        }
    }

    if(dsc->img_src) {
        lv_image_header_t header;
        int image_handle = lv_nanovg_image_cache_get_handle(u, dsc->img_src, lv_color32_make(0, 0, 0, 0), 0, &header);
        if(image_handle < 0) {
            LV_PROFILER_DRAW_END;
            return;
        }

        /* move image to center */
        float img_half_w = header.w / 2.0f;
        float img_half_h = header.h / 2.0f;

        NVGpaint paint = nvgImagePattern(u->vg,
                                         cx - img_half_w, cy - img_half_h,
                                         header.w, header.h, 0,
                                         image_handle,
                                         dsc->opa / (float)LV_OPA_COVER);
        nvgFillPaint(u->vg, paint);
        nvgFill(u->vg);
    }
    else {
        lv_nanovg_fill(u->vg, winding, NVG_SOURCE_OVER, lv_nanovg_color_convert(dsc->color, dsc->opa));
    }

    LV_PROFILER_DRAW_END;
}

/**********************
*   STATIC FUNCTIONS
**********************/

#endif /* LV_USE_DRAW_NANOVG */
