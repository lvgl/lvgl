/**
 * @file lv_draw_vg_lite_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../misc/lv_area_private.h"
#include "../lv_image_decoder_private.h"
#include "../lv_draw_private.h"
#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_math.h"
#include "lv_vg_lite_path.h"
#include "lv_vg_lite_pending.h"
#include "lv_vg_lite_utils.h"
#include <math.h>

/*********************
 *      DEFINES
 *********************/

#define PI 3.1415926535897932384626433832795f
#define TWO_PI 6.283185307179586476925286766559f
#define DEG_TO_RAD 0.017453292519943295769236907684886f
#define RAD_TO_DEG 57.295779513082320876798154814105f
#define radians(deg) ((deg) * DEG_TO_RAD)
#define degrees(rad) ((rad) * RAD_TO_DEG)

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

void lv_draw_vg_lite_arc(lv_draw_unit_t * draw_unit, const lv_draw_arc_dsc_t * dsc,
                         const lv_area_t * coords)
{
    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)draw_unit;

    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, coords, draw_unit->clip_area)) {
        /*Fully clipped, nothing to do*/
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
    if(math_zero(sweep_angle)) {
        return;
    }

    LV_PROFILER_BEGIN;

    lv_vg_lite_path_t * path = lv_vg_lite_path_get(u, VG_LITE_FP32);
    lv_vg_lite_path_set_quality(path, VG_LITE_HIGH);
    lv_vg_lite_path_set_bonding_box_area(path, &clip_area);

    float radius_out = dsc->radius;
    float radius_in = dsc->radius - dsc->width;
    float half_width = dsc->width * 0.5f;
    float radius_center = radius_out - half_width;
    float cx = dsc->center.x;
    float cy = dsc->center.y;

    vg_lite_fill_t fill = VG_LITE_FILL_NON_ZERO;

    if(math_equal(sweep_angle, 360)) {
        lv_vg_lite_path_append_circle(path, cx, cy, radius_out, radius_out);
        lv_vg_lite_path_append_circle(path, cx, cy, radius_in, radius_in);
        fill = VG_LITE_FILL_EVEN_ODD;
    }
    else {
        /* radius_out start point */
        float start_angle_rad = MATH_RADIANS(start_angle);
        float start_x = radius_out * MATH_COSF(start_angle_rad) + cx;
        float start_y = radius_out * MATH_SINF(start_angle_rad) + cy;

        /* radius_in start point */
        float end_angle_rad = MATH_RADIANS(end_angle);
        float end_x = radius_in * MATH_COSF(end_angle_rad) + cx;
        float end_y = radius_in * MATH_SINF(end_angle_rad) + cy;

        lv_vg_lite_path_move_to(path, start_x, start_y);

        /* radius_out arc */
        lv_vg_lite_path_append_arc(path,
                                   cx, cy,
                                   radius_out,
                                   start_angle,
                                   sweep_angle,
                                   false);

        /* line to radius_in */
        lv_vg_lite_path_line_to(path, end_x, end_y);

        /* radius_in arc */
        lv_vg_lite_path_append_arc(path,
                                   cx, cy,
                                   radius_in,
                                   end_angle,
                                   -sweep_angle,
                                   false);

        /* close arc */
        lv_vg_lite_path_close(path);

        /* draw round */
        if(dsc->rounded && half_width > 0) {
            float rcx1 = cx + radius_center * MATH_COSF(end_angle_rad);
            float rcy1 = cy + radius_center * MATH_SINF(end_angle_rad);
            lv_vg_lite_path_append_circle(path, rcx1, rcy1, half_width, half_width);

            float rcx2 = cx + radius_center * MATH_COSF(start_angle_rad);
            float rcy2 = cy + radius_center * MATH_SINF(start_angle_rad);
            lv_vg_lite_path_append_circle(path, rcx2, rcy2, half_width, half_width);
        }
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
                               fill,
                               &matrix,
                               VG_LITE_BLEND_SRC_OVER,
                               color));
    LV_PROFILER_END_TAG("vg_lite_draw");

    if(dsc->img_src) {
        vg_lite_buffer_t src_buf;
        lv_image_decoder_dsc_t decoder_dsc;
        if(lv_vg_lite_buffer_open_image(&src_buf, &decoder_dsc, dsc->img_src, false)) {
            vg_lite_matrix_t path_matrix;
            vg_lite_identity(&path_matrix);
            lv_vg_lite_matrix_multiply(&path_matrix, &u->global_matrix);

            /* move image to center */
            vg_lite_translate(cx - radius_out, cy - radius_out, &matrix);

            LV_VG_LITE_ASSERT_MATRIX(&path_matrix);

            LV_PROFILER_BEGIN_TAG("vg_lite_draw_pattern");
            LV_VG_LITE_CHECK_ERROR(vg_lite_draw_pattern(
                                       &u->target_buffer,
                                       vg_lite_path,
                                       fill,
                                       &path_matrix,
                                       &src_buf,
                                       &matrix,
                                       VG_LITE_BLEND_SRC_OVER,
                                       VG_LITE_PATTERN_COLOR,
                                       0,
                                       color,
                                       VG_LITE_FILTER_BI_LINEAR));
            LV_PROFILER_END_TAG("vg_lite_draw_pattern");
            lv_vg_lite_pending_add(u->image_dsc_pending, &decoder_dsc);
        }
    }

    lv_vg_lite_path_drop(u, path);
    LV_PROFILER_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/
