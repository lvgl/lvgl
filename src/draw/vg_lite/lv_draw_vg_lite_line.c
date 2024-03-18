/**
 * @file lv_draw_vg_lite_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_math.h"
#include "lv_vg_lite_path.h"
#include "lv_vg_lite_utils.h"

/*********************
 *      DEFINES
 *********************/

#define SQ(x) ((x) * (x))

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

void lv_draw_vg_lite_line(lv_draw_unit_t * draw_unit, const lv_draw_line_dsc_t * dsc)
{
    if(dsc->opa <= LV_OPA_MIN)
        return;
    if(dsc->width == 0)
        return;

    float p1_x = dsc->p1.x;
    float p1_y = dsc->p1.y;
    float p2_x = dsc->p2.x;
    float p2_y = dsc->p2.y;

    if(p1_x == p2_x && p1_y == p2_y)
        return;

    float half_w = dsc->width * 0.5f;

    lv_area_t rel_clip_area;
    rel_clip_area.x1 = (int32_t)(LV_MIN(p1_x, p2_x) - half_w);
    rel_clip_area.x2 = (int32_t)(LV_MAX(p1_x, p2_x) + half_w);
    rel_clip_area.y1 = (int32_t)(LV_MIN(p1_y, p2_y) - half_w);
    rel_clip_area.y2 = (int32_t)(LV_MAX(p1_y, p2_y) + half_w);

    if(!_lv_area_intersect(&rel_clip_area, &rel_clip_area, draw_unit->clip_area)) {
        return; /*Fully clipped, nothing to do*/
    }

    LV_PROFILER_BEGIN;

    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)draw_unit;

    int32_t dash_width = dsc->dash_width;
    int32_t dash_gap = dsc->dash_gap;
    int32_t dash_l = dash_width + dash_gap;

    float dx = p2_x - p1_x;
    float dy = p2_y - p1_y;
    float inv_dl = math_fast_inv_sqrtf(SQ(dx) + SQ(dy));
    float w_dx = dsc->width * dy * inv_dl;
    float w_dy = dsc->width * dx * inv_dl;
    float w2_dx = w_dx / 2;
    float w2_dy = w_dy / 2;

    int32_t ndash = 0;
    if(dash_width && dash_l * inv_dl < 1.0f) {
        ndash = (int32_t)((1.0f / inv_dl + dash_l - 1) / dash_l);
    }

    lv_vg_lite_path_t * path = lv_vg_lite_path_get(u, VG_LITE_FP32);
    lv_vg_lite_path_set_quality(path, VG_LITE_MEDIUM);
    lv_vg_lite_path_set_bonding_box_area(path, &rel_clip_area);

    /* head point */
    float head_start_x = p1_x + w2_dx;
    float head_start_y = p1_y - w2_dy;
    float head_end_x = p1_x - w2_dx;
    float head_end_y = p1_y + w2_dy;

    /* tail point */
    float tail_start_x = p2_x - w2_dx;
    float tail_start_y = p2_y + w2_dy;
    float tail_end_x = p2_x + w2_dx;
    float tail_end_y = p2_y - w2_dy;

    /*
          head_start        tail_end
              *-----------------*
             /|                 |\
            / |                 | \
    arc_c *(  *p1             p2*  )* arc_c
            \ |                 | /
             \|                 |/
              *-----------------*
          head_end          tail_start
    */

    /* move to start point */
    lv_vg_lite_path_move_to(path, head_start_x, head_start_y);

    /* draw line head */
    if(dsc->round_start) {
        float arc_cx = p1_x - w2_dy;
        float arc_cy = p1_y - w2_dx;

        /* start 90deg arc */
        lv_vg_lite_path_append_arc_right_angle(path,
                                               head_start_x, head_start_y,
                                               p1_x, p1_y,
                                               arc_cx, arc_cy);

        /* end 90deg arc */
        lv_vg_lite_path_append_arc_right_angle(path,
                                               arc_cx, arc_cy,
                                               p1_x, p1_y,
                                               head_end_x, head_end_y);
    }
    else {
        lv_vg_lite_path_line_to(path, head_end_x, head_end_y);
    }

    /* draw line body */
    lv_vg_lite_path_line_to(path, tail_start_x, tail_start_y);

    /* draw line tail */
    if(dsc->round_end) {
        float arc_cx = p2_x + w2_dy;
        float arc_cy = p2_y + w2_dx;
        lv_vg_lite_path_append_arc_right_angle(path,
                                               tail_start_x, tail_start_y,
                                               p2_x, p2_y,
                                               arc_cx, arc_cy);
        lv_vg_lite_path_append_arc_right_angle(path,
                                               arc_cx, arc_cy,
                                               p2_x, p2_y,
                                               tail_end_x, tail_end_y);
    }
    else {
        lv_vg_lite_path_line_to(path, tail_end_x, tail_end_y);
    }

    /* close draw line body */
    lv_vg_lite_path_line_to(path, head_start_x, head_start_y);

    for(int32_t i = 0; i < ndash; i++) {
        float start_x = p1_x - w2_dx + dx * (i * dash_l + dash_width) * inv_dl;
        float start_y = p1_y + w2_dy + dy * (i * dash_l + dash_width) * inv_dl;

        lv_vg_lite_path_move_to(path, start_x, start_y);
        lv_vg_lite_path_line_to(path,
                                p1_x + w2_dx + dx * (i * dash_l + dash_width) * inv_dl,
                                p1_y - w2_dy + dy * (i * dash_l + dash_width) * inv_dl);
        lv_vg_lite_path_line_to(path,
                                p1_x + w2_dx + dx * (i + 1) * dash_l * inv_dl,
                                p1_y - w2_dy + dy * (i + 1) * dash_l * inv_dl);
        lv_vg_lite_path_line_to(path,
                                p1_x - w2_dx + dx * (i + 1) * dash_l * inv_dl,
                                p1_y + w2_dy + dy * (i + 1) * dash_l * inv_dl);
        lv_vg_lite_path_line_to(path, start_x, start_y);
    }

    lv_vg_lite_path_end(path);

    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    lv_vg_lite_matrix_multiply(&matrix, &u->global_matrix);

    vg_lite_color_t color = lv_vg_lite_color(dsc->color, dsc->opa, true);

    vg_lite_path_t * vg_lite_path = lv_vg_lite_path_get_path(path);

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

#endif /*LV_USE_DRAW_VG_LITE*/
