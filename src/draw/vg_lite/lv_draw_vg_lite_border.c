/**
 * @file lv_draw_vg_lite_border.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_utils.h"
#include "lv_vg_lite_path.h"

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

void lv_draw_vg_lite_border(lv_draw_unit_t * draw_unit, const lv_draw_border_dsc_t * dsc,
                            const lv_area_t * coords)
{
    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)draw_unit;

    lv_area_t clip_area;
    if(!_lv_area_intersect(&clip_area, coords, draw_unit->clip_area)) {
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

    int32_t border_w = dsc->width;
    float r_in = LV_MAX(0, r_out - border_w);

    lv_vg_lite_path_t * path = lv_vg_lite_path_get(u, VG_LITE_FP32);
    lv_vg_lite_path_set_quality(path, dsc->radius == 0 ? VG_LITE_LOW : VG_LITE_HIGH);
    lv_vg_lite_path_set_bonding_box_area(path, &clip_area);

    /* outer rect */
    lv_vg_lite_path_append_rect(path,
                                coords->x1, coords->y1,
                                w, h,
                                r_out, r_out);

    /* inner rect */
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
                                coords->x1 + x_offset,
                                coords->y1 + y_offset,
                                w_inner,
                                h_inner,
                                r_in, r_in);

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
