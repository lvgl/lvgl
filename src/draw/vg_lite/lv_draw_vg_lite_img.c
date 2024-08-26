/**
 * @file lv_draw_vg_lite_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../misc/lv_area_private.h"
#include "../lv_image_decoder_private.h"
#include "../lv_draw_image_private.h"
#include "../lv_draw_private.h"
#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_decoder.h"
#include "lv_vg_lite_path.h"
#include "lv_vg_lite_pending.h"
#include "lv_vg_lite_utils.h"

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

void lv_draw_vg_lite_img(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * dsc,
                         const lv_area_t * coords, bool no_cache)
{
    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)draw_unit;

    /* The coordinates passed in by coords are not transformed,
     * so the transformed area needs to be calculated once.
     */
    lv_area_t image_tf_area;
    lv_image_buf_get_transformed_area(
        &image_tf_area,
        lv_area_get_width(coords),
        lv_area_get_height(coords),
        dsc->rotation,
        dsc->scale_x,
        dsc->scale_y,
        &dsc->pivot);
    lv_area_move(&image_tf_area, coords->x1, coords->y1);

    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, &image_tf_area, draw_unit->clip_area)) {
        /*Fully clipped, nothing to do*/
        return;
    }

    LV_PROFILER_BEGIN;

    vg_lite_buffer_t src_buf;
    lv_image_decoder_dsc_t decoder_dsc;
    if(!lv_vg_lite_buffer_open_image(&src_buf, &decoder_dsc, dsc->src, no_cache)) {
        LV_PROFILER_END;
        return;
    }

    vg_lite_color_t color = 0;
    if(LV_COLOR_FORMAT_IS_ALPHA_ONLY(decoder_dsc.decoded->header.cf) || dsc->recolor_opa > LV_OPA_TRANSP) {
        /* alpha image and image recolor */
        src_buf.image_mode = VG_LITE_MULTIPLY_IMAGE_MODE;
        color = lv_vg_lite_color(dsc->recolor, LV_OPA_MIX2(dsc->opa, dsc->recolor_opa), true);
    }
    else if(dsc->opa < LV_OPA_COVER) {
        /* normal image opa */
        src_buf.image_mode = VG_LITE_MULTIPLY_IMAGE_MODE;
        lv_memset(&color, dsc->opa, sizeof(color));
    }

    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    lv_vg_lite_matrix_multiply(&matrix, &u->global_matrix);
    lv_vg_lite_image_matrix(&matrix, coords->x1, coords->y1, dsc);

    LV_VG_LITE_ASSERT_SRC_BUFFER(&src_buf);
    LV_VG_LITE_ASSERT_DEST_BUFFER(&u->target_buffer);

    bool no_transform = lv_matrix_is_identity_or_translation((const lv_matrix_t *)&matrix);
    vg_lite_filter_t filter = no_transform ? VG_LITE_FILTER_POINT : VG_LITE_FILTER_BI_LINEAR;

    /* If clipping is not required, blit directly */
    if(lv_area_is_in(&image_tf_area, draw_unit->clip_area, false) && dsc->clip_radius <= 0) {
        /* The image area is the coordinates relative to the image itself */
        lv_area_t src_area = *coords;
        lv_area_move(&src_area, -coords->x1, -coords->y1);

        /* rect is used to crop the pixel-aligned padding area */
        vg_lite_rectangle_t rect;
        lv_vg_lite_rect(&rect, &src_area);

        LV_PROFILER_BEGIN_TAG("vg_lite_blit_rect");
        LV_VG_LITE_CHECK_ERROR(vg_lite_blit_rect(
                                   &u->target_buffer,
                                   &src_buf,
                                   &rect,
                                   &matrix,
                                   lv_vg_lite_blend_mode(dsc->blend_mode),
                                   color,
                                   filter));
        LV_PROFILER_END_TAG("vg_lite_blit_rect");
    }
    else {
        lv_vg_lite_path_t * path = lv_vg_lite_path_get(u, VG_LITE_FP32);

        if(dsc->clip_radius) {
            int32_t width = lv_area_get_width(coords);
            int32_t height = lv_area_get_height(coords);
            float r_short = LV_MIN(width, height) / 2.0f;
            float radius = LV_MIN(dsc->clip_radius, r_short);

            /**
             * When clip_radius is enabled, the clipping edges
             * are aligned with the image edges
             */
            lv_vg_lite_path_append_rect(
                path,
                coords->x1, coords->y1,
                width, height,
                radius);
        }
        else {
            lv_vg_lite_path_append_rect(
                path,
                clip_area.x1, clip_area.y1,
                lv_area_get_width(&clip_area), lv_area_get_height(&clip_area),
                0);
        }

        lv_vg_lite_path_set_bonding_box_area(path, &clip_area);
        lv_vg_lite_path_end(path);

        vg_lite_path_t * vg_lite_path = lv_vg_lite_path_get_path(path);
        LV_VG_LITE_ASSERT_PATH(vg_lite_path);

        vg_lite_matrix_t path_matrix;
        vg_lite_identity(&path_matrix);
        lv_vg_lite_matrix_multiply(&path_matrix, &u->global_matrix);

        LV_PROFILER_BEGIN_TAG("vg_lite_draw_pattern");
        LV_VG_LITE_CHECK_ERROR(vg_lite_draw_pattern(
                                   &u->target_buffer,
                                   vg_lite_path,
                                   VG_LITE_FILL_EVEN_ODD,
                                   &path_matrix,
                                   &src_buf,
                                   &matrix,
                                   lv_vg_lite_blend_mode(dsc->blend_mode),
                                   VG_LITE_PATTERN_COLOR,
                                   0,
                                   color,
                                   filter));
        LV_PROFILER_END_TAG("vg_lite_draw_pattern");

        lv_vg_lite_path_drop(u, path);
    }

    lv_vg_lite_pending_add(u->image_dsc_pending, &decoder_dsc);
    LV_PROFILER_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/
