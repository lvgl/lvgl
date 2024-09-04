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

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void path_append_inner_rect(lv_vg_lite_path_t * path,
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

void lv_draw_vg_lite_border(lv_draw_unit_t * draw_unit, const lv_draw_border_dsc_t * dsc,
                            const lv_area_t * coords)
{
    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)draw_unit;

    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, coords, draw_unit->clip_area)) {
        /*Fully clipped, nothing to do*/
        return;
    }

    LV_PROFILER_BEGIN;

    int32_t w = lv_area_get_width(coords);
    int32_t h = lv_area_get_height(coords);
    float r_out = dsc->radius;
    if(dsc->radius) {
        float r_short = LV_MIN(w, h) / 2.0f;
        r_out = LV_MIN(r_out, r_short);
    }

    lv_vg_lite_path_t * path = lv_vg_lite_path_get(u, VG_LITE_FP32);
    lv_vg_lite_path_set_quality(path, dsc->radius == 0 ? VG_LITE_LOW : VG_LITE_HIGH);
    lv_vg_lite_path_set_bonding_box_area(path, &clip_area);

    /* outer rect */
    lv_vg_lite_path_append_rect(path,
                                coords->x1, coords->y1,
                                w, h,
                                r_out);

    /* inner rect */
    path_append_inner_rect(path, dsc, coords->x1, coords->y1, w, h, r_out);

    lv_vg_lite_path_end(path);

    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    lv_vg_lite_matrix_multiply(&matrix, &u->global_matrix);

    vg_lite_color_t color = lv_vg_lite_color(dsc->color, dsc->opa, true);

    vg_lite_path_t * vg_lite_path = lv_vg_lite_path_get_path(path);

    lv_vg_lite_path_dump_info(vg_lite_path);

    LV_VG_LITE_ASSERT_DEST_BUFFER(&u->target_buffer);
    LV_VG_LITE_ASSERT_PATH(vg_lite_path);
    LV_VG_LITE_ASSERT_MATRIX(&matrix);

    LV_PROFILER_BEGIN_TAG("vg_lite_draw");
    LV_VG_LITE_CHECK_ERROR(vg_lite_draw(
                               &u->target_buffer,
                               vg_lite_path,
                               VG_LITE_FILL_EVEN_ODD,
                               &matrix,
                               VG_LITE_BLEND_SRC_OVER,
                               color));
    LV_PROFILER_END_TAG("vg_lite_draw");

    lv_vg_lite_path_drop(u, path);
    LV_PROFILER_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void path_append_inner_rect(lv_vg_lite_path_t * path,
                                   const lv_draw_border_dsc_t * dsc,
                                   int32_t x, int32_t y, int32_t w, int32_t h,
                                   float r)
{
    int32_t border_w = dsc->width;
    float r_in = LV_MAX(0, r - border_w);

    /* full border, simple rect */
    if(dsc->side == LV_BORDER_SIDE_FULL) {
        lv_vg_lite_path_append_rect(path,
                                    x + border_w, y + border_w,
                                    w - border_w * 2, h - border_w * 2,
                                    r_in);
        return;
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
        return;
    }

    /* complex rounded compound rectangle */

    /* r^2 - r_in^2 = offset^2 */
    float offset = MATH_SQRTF((2 * r_in) * border_w);
    float sweep_alpha = MATH_DEGREES(MATH_ACOSF(r_in / r));
    float sweep_beta = 90 - sweep_alpha;

    bool has_top = dsc->side & LV_BORDER_SIDE_TOP;
    bool has_left = dsc->side & LV_BORDER_SIDE_LEFT;
    bool has_bottom = dsc->side & LV_BORDER_SIDE_BOTTOM;
    bool has_right = dsc->side & LV_BORDER_SIDE_RIGHT;

    float c1_x = x + r;
    float c1_y = y + r;
    float c2_x = x + w - r;
    float c2_y = c1_y;
    float c3_x = c2_x;
    float c3_y = y + h - r;
    float c4_x = c1_x;
    float c4_y = c3_y;

    /* top left corner */
    {
        float p1_x = x + border_w;
        float p1_y = y + r;
        float p2_x = x;
        float p2_y = y + r;
        float p3_x = x + border_w;
        float p3_y = y + r - offset;
        // float p4_x = x + r - offset;
        // float p4_y = y + border_w;
        float p5_x = x + r;
        float p5_y = y;
        float p6_x = x + r;
        float p6_y = y + border_w;

        if(has_left && has_top) {
            lv_vg_lite_path_move_to(path, p1_x, p1_y);
            lv_vg_lite_path_append_arc_right_angle(path, p1_x, p1_y, c1_x, c1_y, p6_x, p6_y);
        }
        else if(has_left && !has_top) {
            lv_vg_lite_path_move_to(path, p1_x, p1_y);
            lv_vg_lite_path_line_to(path, p3_x, p3_y);
            /* to p5 */
            lv_vg_lite_path_append_arc(path, c1_x, c1_y, r, 180 + sweep_alpha, sweep_beta, false);
        }
        else if(!has_left && has_top) {
            lv_vg_lite_path_move_to(path, p2_x, p2_y);
            /* to p4 */
            lv_vg_lite_path_append_arc(path, c1_x, c1_y, r, 180, sweep_beta, false);
        }
        else if(!has_left && !has_top) {
            lv_vg_lite_path_move_to(path, p2_x, p2_y);
            lv_vg_lite_path_append_arc_right_angle(path, p2_x, p2_y, c1_x, c1_y, p5_x, p5_y);
        }
    }

    /* top right corner */
    {
        float p7_x = x + w - r;
        float p7_y = y + border_w;
        float p8_x = x + w - r;
        float p8_y = y;
        float p9_x = x + w - r + offset;
        float p9_y = y + border_w;
        // float p10_x = x + w - border_w;
        // float p10_y = y + r - offset;
        float p11_x = x + w;
        float p11_y = y + r;
        float p12_x = x + w - border_w;
        float p12_y = y + r;

        if(has_top && has_right) {
            lv_vg_lite_path_line_to(path, p7_x, p7_y);
            lv_vg_lite_path_append_arc_right_angle(path, p7_x, p7_y, c2_x, c2_y, p12_x, p12_y);
        }
        else if(has_top && !has_right) {
            lv_vg_lite_path_line_to(path, p9_x, p9_y);
            lv_vg_lite_path_append_arc(path, c2_x, c2_y, r, 360 - sweep_beta, sweep_beta, false);
        }
        else if(!has_top && has_right) {
            lv_vg_lite_path_line_to(path, p8_x, p8_y);
            /* to p10 */
            lv_vg_lite_path_append_arc(path, c2_x, c2_y, r, 270, sweep_beta, false);
        }
        else if(!has_top && !has_right) {
            lv_vg_lite_path_line_to(path, p8_x, p8_y);
            lv_vg_lite_path_append_arc_right_angle(path, p8_x, p8_y, c2_x, c2_y, p11_x, p11_y);
        }
    }

    /* bottom right corner */
    {
        float p13_x = x + w - border_w;
        float p13_y = y + h - r;
        float p14_x = x + w;
        float p14_y = y + h - r;
        float p15_x = x + w - border_w;
        float p15_y = y + h - r + offset;
        // float p16_x = x + w - r + offset;
        // float p16_y = y + h - border_w;
        float p17_x = x + w - r;
        float p17_y = y + h;
        float p18_x = x + w - r;
        float p18_y = y + h - border_w;

        if(has_right && has_bottom) {
            lv_vg_lite_path_line_to(path, p13_x, p13_y);
            lv_vg_lite_path_append_arc_right_angle(path, p13_x, p13_y, c3_x, c3_y, p18_x, p18_y);
        }
        else if(has_right && !has_bottom) {
            lv_vg_lite_path_line_to(path, p15_x, p15_y);
            /* to p17 */
            lv_vg_lite_path_append_arc(path, c3_x, c3_y, r, sweep_alpha, sweep_beta, false);
        }
        else if(!has_right && has_bottom) {
            lv_vg_lite_path_line_to(path, p14_x, p14_y);
            /* to p16 */
            lv_vg_lite_path_append_arc(path, c3_x, c3_y, r, 0, sweep_beta, false);
        } else if(!has_right && !has_bottom) {
            lv_vg_lite_path_line_to(path, p14_x, p14_y);
            lv_vg_lite_path_append_arc_right_angle(path, p14_x, p14_y, c3_x, c3_y, p17_x, p17_y);
        }
    }

    /* left bottom corner */
    {
        float p19_x = x + r;
        float p19_y = y + h - border_w;
        float p20_x = x + r;
        float p20_y = y + h;
        float p21_x = x + r - offset;
        float p21_y = y + h - border_w;
        // float p22_x = x + border_w;
        // float p22_y = y + h - r + offset;
        float p23_x = x;
        float p23_y = y + h - r;
        float p24_x = x + border_w;
        float p24_y = y + h - r;

        if(has_bottom && has_left) {
            lv_vg_lite_path_line_to(path, p19_x, p19_y);
            lv_vg_lite_path_append_arc_right_angle(path, p19_x, p19_y, c4_x, c4_y, p24_x, p24_y);
        }
        else if(has_bottom && !has_left) {
            lv_vg_lite_path_line_to(path, p21_x, p21_y);
            /* to p23 */
            lv_vg_lite_path_append_arc(path, c4_x, c4_y, r, 90 + sweep_alpha, sweep_beta, false);
        } else if(!has_bottom && has_left) {
            lv_vg_lite_path_line_to(path, p20_x, p20_y);
            /* to p22 */
            lv_vg_lite_path_append_arc(path, c4_x, c4_y, r, 90, sweep_beta, false);
        } else if(!has_bottom && !has_left) {
            lv_vg_lite_path_line_to(path, p20_x, p20_y);
            lv_vg_lite_path_append_arc_right_angle(path, p20_x, p20_y, c4_x, c4_y, p23_x, p23_y);
        }
    }

    lv_vg_lite_path_close(path);
}

#endif /*LV_USE_DRAW_VG_LITE*/
