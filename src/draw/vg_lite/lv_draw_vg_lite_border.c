/**
 * @file lv_draw_vg_lite_border.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../misc/lv_area_private.h"
#include "../lv_draw_private.h"
#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_utils.h"
#include "lv_vg_lite_path.h"
#include "lv_vg_lite_math.h"

/*********************
 *      DEFINES
 *********************/

#define HAS_BORDER_SIDE(dsc_side, side) (((dsc_side) & (side)) == (side))

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static vg_lite_fill_t path_append_inner_rect(lv_vg_lite_path_t * path,
                                             const lv_draw_border_dsc_t * dsc,
                                             int32_t x, int32_t y, int32_t w, int32_t h,
                                             float r);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_vg_lite_border(lv_draw_task_t * t, const lv_draw_border_dsc_t * dsc,
                            const lv_area_t * coords)
{
    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)t->draw_unit;

    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, coords, &t->clip_area)) {
        /*Fully clipped, nothing to do*/
        return;
    }

    LV_PROFILER_DRAW_BEGIN;

    int32_t w = lv_area_get_width(coords);
    int32_t h = lv_area_get_height(coords);
    float r_out = dsc->radius;
    if(dsc->radius) {
        float r_short = LV_MIN(w, h) / 2.0f;
        r_out = LV_MIN(r_out, r_short);
    }

    lv_vg_lite_path_t * path = lv_vg_lite_path_get(u, VG_LITE_FP32);
    lv_vg_lite_path_set_quality(path, dsc->radius == 0 ? VG_LITE_LOW : VG_LITE_HIGH);
    lv_vg_lite_path_set_bounding_box_area(path, &clip_area);

    /* outer rect */
    lv_vg_lite_path_append_rect(path,
                                coords->x1, coords->y1,
                                w, h,
                                r_out);

    /* inner rect */
    vg_lite_fill_t fill_rule = path_append_inner_rect(path, dsc, coords->x1, coords->y1, w, h, r_out);

    lv_vg_lite_path_end(path);

    vg_lite_matrix_t matrix = u->global_matrix;

    lv_vg_lite_draw(
        &u->target_buffer,
        lv_vg_lite_path_get_path(path),
        fill_rule,
        &matrix,
        VG_LITE_BLEND_SRC_OVER,
        lv_vg_lite_color(dsc->color, dsc->opa, true));

    lv_vg_lite_path_drop(u, path);
    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static vg_lite_fill_t path_append_inner_rect(lv_vg_lite_path_t * path,
                                             const lv_draw_border_dsc_t * dsc,
                                             int32_t x, int32_t y, int32_t w, int32_t h,
                                             float r)
{
    LV_PROFILER_DRAW_BEGIN;

    const float half_w = w / 2.0f;
    const float half_h = h / 2.0f;
    const int32_t border_w = dsc->width;
    const float border_w_max = LV_MIN(half_w, half_h);

    /* normal fill, no inner rect */
    if(border_w >= border_w_max) {
        LV_PROFILER_DRAW_END;
        return VG_LITE_FILL_EVEN_ODD;
    }

    const float r_in = r - border_w;

    /* full border, simple rect */
    if(dsc->side == LV_BORDER_SIDE_FULL) {
        lv_vg_lite_path_append_rect(path,
                                    x + border_w, y + border_w,
                                    w - border_w * 2, h - border_w * 2,
                                    r_in < 0 ? 0 : r_in);
        LV_PROFILER_DRAW_END;
        return VG_LITE_FILL_EVEN_ODD;
    }

    /* no-radius case, simple inner rect */
    if(dsc->radius <= 0) {
        int32_t x_offset = 0;
        int32_t y_offset = 0;
        int32_t w_inner = w;
        int32_t h_inner = h;

        if(dsc->side & LV_BORDER_SIDE_TOP) {
            y_offset += border_w;
            h_inner -= border_w;
        }
        if(dsc->side & LV_BORDER_SIDE_LEFT) {
            x_offset += border_w;
            w_inner -= border_w;
        }
        if(dsc->side & LV_BORDER_SIDE_BOTTOM) {
            h_inner -= border_w;
        }
        if(dsc->side & LV_BORDER_SIDE_RIGHT) {
            w_inner -= border_w;
        }

        lv_vg_lite_path_append_rect(path,
                                    x + x_offset,
                                    y + y_offset,
                                    w_inner,
                                    h_inner,
                                    0);
        LV_PROFILER_DRAW_END;
        return VG_LITE_FILL_EVEN_ODD;
    }

    /* reset outter rect path */
    lv_vg_lite_path_reset(path, VG_LITE_FP32);

    /* coordinate reference map: https://github.com/lvgl/lvgl/pull/6796 */
    const float c1_x = x + r;
    const float c1_y = y + r;
    const float c2_x = x + w - r;
    const float c2_y = c1_y;
    const float c3_x = c2_x;
    const float c3_y = y + h - r;
    const float c4_x = c1_x;
    const float c4_y = c3_y;

    /* When border_w > r, No need to calculate the intersection of the arc and the line */
    if(r_in <= 0) {
        const float p1_x = x;
        const float p1_y = y + border_w;
        const float p2_x = x;
        const float p2_y = y + r;
        const float p3_x = x + r;
        const float p3_y = y;
        const float p4_x = x + border_w;
        const float p4_y = y;

        const float p5_x = x + w - border_w;
        const float p5_y = y;
        const float p6_x = x + w - r;
        const float p6_y = y;
        const float p7_x = x + w;
        const float p7_y = y + r;
        const float p8_x = x + w;
        const float p8_y = y + border_w;

        const float p9_x = x + w;
        const float p9_y = y + h - border_w;
        const float p10_x = x + w;
        const float p10_y = y + h - r;
        const float p11_x = x + w - r;
        const float p11_y = y + h;
        const float p12_x = x + w - border_w;
        const float p12_y = y + h;

        const float p13_x = x + border_w;
        const float p13_y = y + h;
        const float p14_x = x + r;
        const float p14_y = y + h;
        const float p15_x = x;
        const float p15_y = y + h - r;
        const float p16_x = x;
        const float p16_y = y + h - border_w;

        if(dsc->side & LV_BORDER_SIDE_BOTTOM) {
            lv_vg_lite_path_move_to(path, p16_x, p16_y);
            lv_vg_lite_path_line_to(path, p9_x, p9_y);
            lv_vg_lite_path_line_to(path, p10_x, p10_y);
            lv_vg_lite_path_append_arc_right_angle(path, p10_x, p10_y, c3_x, c3_y, p11_x, p11_y);
            lv_vg_lite_path_line_to(path, p14_x, p14_y);
            lv_vg_lite_path_append_arc_right_angle(path, p14_x, p14_y, c4_x, c4_y, p15_x, p15_y);
            lv_vg_lite_path_close(path);
        }

        if(dsc->side & LV_BORDER_SIDE_TOP) {
            lv_vg_lite_path_move_to(path, p1_x, p1_y);
            lv_vg_lite_path_line_to(path, p2_x, p2_y);
            lv_vg_lite_path_append_arc_right_angle(path, p2_x, p2_y, c1_x, c1_y, p3_x, p3_y);
            lv_vg_lite_path_line_to(path, p6_x, p6_y);
            lv_vg_lite_path_append_arc_right_angle(path, p6_x, p6_y, c2_x, c2_y, p7_x, p7_y);
            lv_vg_lite_path_line_to(path, p8_x, p8_y);
            lv_vg_lite_path_close(path);
        }

        if(dsc->side & LV_BORDER_SIDE_LEFT) {
            lv_vg_lite_path_move_to(path, p4_x, p4_y);
            lv_vg_lite_path_line_to(path, p13_x, p13_y);
            lv_vg_lite_path_line_to(path, p14_x, p14_y);
            lv_vg_lite_path_append_arc_right_angle(path, p14_x, p14_y, c4_x, c4_y, p15_x, p15_y);
            lv_vg_lite_path_line_to(path, p2_x, p2_y);
            lv_vg_lite_path_append_arc_right_angle(path, p2_x, p2_y, c1_x, c1_y, p3_x, p3_y);
            lv_vg_lite_path_close(path);
        }

        if(dsc->side & LV_BORDER_SIDE_RIGHT) {
            lv_vg_lite_path_move_to(path, p5_x, p5_y);
            lv_vg_lite_path_line_to(path, p6_x, p6_y);
            lv_vg_lite_path_append_arc_right_angle(path, p6_x, p6_y, c2_x, c2_y, p7_x, p7_y);
            lv_vg_lite_path_line_to(path, p10_x, p10_y);
            lv_vg_lite_path_append_arc_right_angle(path, p10_x, p10_y, c3_x, c3_y, p11_x, p11_y);
            lv_vg_lite_path_line_to(path, p12_x, p12_y);
            lv_vg_lite_path_close(path);
        }

        LV_PROFILER_DRAW_END;
        return VG_LITE_FILL_NON_ZERO;
    }

    /* When border_w < r, Calculate the intersection of an arc and a line */

    /* r^2 - r_in^2 = offset^2 */
    const float offset = MATH_SQRTF((2 * r - border_w) * border_w);
    const float sweep_alpha = MATH_DEGREES(MATH_ACOSF(r_in / r));
    const float sweep_beta = 90 - sweep_alpha;

    const float p1_x = x + border_w;
    const float p1_y = y + r;
    const float p2_x = x;
    const float p2_y = y + r;
    const float p3_x = x + border_w;
    const float p3_y = y + r - offset;
    const float p4_x = x + r - offset;
    const float p4_y = y + border_w;
    const float p5_x = x + r;
    const float p5_y = y;
    const float p6_x = x + r;
    const float p6_y = y + border_w;

    const float p7_x = x + w - r;
    const float p7_y = y + border_w;
    const float p8_x = x + w - r;
    const float p8_y = y;
    const float p10_x = x + w - border_w;
    const float p10_y = y + r - offset;
    const float p11_x = x + w;
    const float p11_y = y + r;
    const float p12_x = x + w - border_w;
    const float p12_y = y + r;

    const float p13_x = x + w - border_w;
    const float p13_y = y + h - r;
    const float p14_x = x + w;
    const float p14_y = y + h - r;
    const float p16_x = x + w - r + offset;
    const float p16_y = y + h - border_w;
    const float p17_x = x + w - r;
    const float p17_y = y + h;
    const float p18_x = x + w - r;
    const float p18_y = y + h - border_w;

    const float p19_x = x + r;
    const float p19_y = y + h - border_w;
    const float p20_x = x + r;
    const float p20_y = y + h;
    const float p21_x = x + r - offset;
    const float p21_y = y + h - border_w;
    const float p22_x = x + border_w;
    const float p22_y = y + h - r + offset;
    const float p23_x = x;
    const float p23_y = y + h - r;
    const float p24_x = x + border_w;
    const float p24_y = y + h - r;

    if(dsc->side & LV_BORDER_SIDE_BOTTOM) {
        lv_vg_lite_path_move_to(path, p21_x, p21_y);
        lv_vg_lite_path_line_to(path, p16_x, p16_y);
        lv_vg_lite_path_append_arc(path, c3_x, c3_y, r, sweep_beta, sweep_alpha, false);
        lv_vg_lite_path_line_to(path, p20_x, p20_y);
        lv_vg_lite_path_append_arc(path, c4_x, c4_y, r, 90, sweep_alpha, false);
        lv_vg_lite_path_close(path);
    }

    if(dsc->side & LV_BORDER_SIDE_TOP) {
        lv_vg_lite_path_move_to(path, p4_x, p4_y);
        lv_vg_lite_path_append_arc(path, c1_x, c1_y, r, 270 - sweep_alpha, sweep_alpha, false);
        lv_vg_lite_path_line_to(path, p8_x, p8_y);
        lv_vg_lite_path_append_arc(path, c2_x, c2_y, r, 270, sweep_alpha, false);
        lv_vg_lite_path_close(path);
    }

    if(dsc->side & LV_BORDER_SIDE_LEFT) {
        lv_vg_lite_path_move_to(path, p3_x, p3_y);
        lv_vg_lite_path_line_to(path, p22_x, p22_y);
        lv_vg_lite_path_append_arc(path, c4_x, c4_y, r, 90 + sweep_beta, sweep_alpha, false);
        lv_vg_lite_path_line_to(path, p2_x, p2_y);
        lv_vg_lite_path_append_arc(path, c1_x, c1_y, r, 180, sweep_alpha, false);
        lv_vg_lite_path_close(path);
    }

    if(dsc->side & LV_BORDER_SIDE_RIGHT) {
        lv_vg_lite_path_move_to(path, p10_x, p10_y);
        lv_vg_lite_path_append_arc(path, c2_x, c2_y, r, 270 + sweep_beta, sweep_alpha, false);
        lv_vg_lite_path_line_to(path, p14_x, p14_y);
        lv_vg_lite_path_append_arc(path, c3_x, c3_y, r, 0, sweep_alpha, false);
        lv_vg_lite_path_close(path);
    }

    /* Draw the rounded corners adjacent to the border */

    if(HAS_BORDER_SIDE(dsc->side, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_LEFT)) {
        lv_vg_lite_path_move_to(path, p2_x, p2_y);
        lv_vg_lite_path_append_arc_right_angle(path, p2_x, p2_y, c1_x, c1_y, p5_x, p5_y);
        lv_vg_lite_path_line_to(path, p6_x, p6_y);
        lv_vg_lite_path_append_arc_right_angle(path, p6_x, p6_y, c1_x, c1_y, p1_x, p1_y);
        lv_vg_lite_path_close(path);
    }

    if(HAS_BORDER_SIDE(dsc->side, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_RIGHT)) {
        lv_vg_lite_path_move_to(path, p8_x, p8_y);
        lv_vg_lite_path_append_arc_right_angle(path, p8_x, p8_y, c2_x, c2_y, p11_x, p11_y);
        lv_vg_lite_path_line_to(path, p12_x, p12_y);
        lv_vg_lite_path_append_arc_right_angle(path, p12_x, p12_y, c2_x, c2_y, p7_x, p7_y);
        lv_vg_lite_path_close(path);
    }

    if(HAS_BORDER_SIDE(dsc->side, LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_LEFT)) {
        lv_vg_lite_path_move_to(path, p20_x, p20_y);
        lv_vg_lite_path_append_arc_right_angle(path, p20_x, p20_y, c4_x, c4_y, p23_x, p23_y);
        lv_vg_lite_path_line_to(path, p24_x, p24_y);
        lv_vg_lite_path_append_arc_right_angle(path, p24_x, p24_y, c4_x, c4_y, p19_x, p19_y);
        lv_vg_lite_path_close(path);
    }

    if(HAS_BORDER_SIDE(dsc->side, LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_RIGHT)) {
        lv_vg_lite_path_move_to(path, p14_x, p14_y);
        lv_vg_lite_path_append_arc_right_angle(path, p14_x, p14_y, c3_x, c3_y, p17_x, p17_y);
        lv_vg_lite_path_line_to(path, p18_x, p18_y);
        lv_vg_lite_path_append_arc_right_angle(path, p18_x, p18_y, c3_x, c3_y, p13_x, p13_y);
        lv_vg_lite_path_close(path);
    }

    LV_PROFILER_DRAW_END;
    return VG_LITE_FILL_NON_ZERO;
}

#endif /*LV_USE_DRAW_VG_LITE*/
