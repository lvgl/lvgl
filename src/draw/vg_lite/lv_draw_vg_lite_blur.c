/**
 * @file lv_draw_vg_lite_blur.c
 *
 * VG-Lite blur implementation.
 *
 * The blur is approximated by one bilinear downscale/upscale round trip whose
 * scale is derived from the blur radius. The temporary buffer keeps the color
 * format of the layer.
 *
 * Coordinate spaces:
 * - logical: LVGL coordinates (coords, clip_area), before any rotation
 * - physical: pixels of the layer buffer, physical = G * logical, where G is
 *   u->global_matrix (buf_area offset + layer matrix, e.g. screen rotation)
 * - small: pixels of the temporary buffer, aligned with the logical ROI, so
 *   the blur is independent of the screen rotation
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_path.h"
#include "lv_vg_lite_utils.h"
#include "../lv_draw_private.h"
#include "../../misc/lv_area_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static int32_t get_downsample_factor(int32_t blur_radius);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_vg_lite_blur(lv_draw_task_t * t, const lv_draw_blur_dsc_t * dsc,
                          const lv_area_t * coords)
{
    if(dsc->blur_radius <= 0) return;

    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)t->draw_unit;

    /* Logical coordinates. The complete ROI defines the blur geometry so that
     * a partial clip does not change the downscale size or shift the result. */
    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, coords, &t->clip_area)) return;

    const lv_area_t * roi = coords;
    const int32_t width = lv_area_get_width(roi);
    const int32_t height = lv_area_get_height(roi);
    if(width <= 0 || height <= 0) return;

    LV_PROFILER_DRAW_BEGIN;

    const int32_t factor = get_downsample_factor(dsc->blur_radius);
    /* Ceil division, width and height are positive */
    const int32_t small_width = (width + factor - 1) / factor;
    const int32_t small_height = (height + factor - 1) / factor;

    /* Exact ratios, so the ROI maps onto [0, small_w] x [0, small_h] */
    const float scale_x = (float)small_width / width;
    const float scale_y = (float)small_height / height;

    vg_lite_matrix_t inv_global;
    if(!lv_vg_lite_matrix_inverse(&inv_global, &u->global_matrix)) {
        LV_LOG_WARN("global matrix is not invertible:");
        lv_vg_lite_matrix_dump_info(&u->global_matrix);
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_draw_buf_t * small = lv_draw_buf_create(small_width, small_height,
                                               t->target_layer->draw_buf->header.cf, LV_STRIDE_AUTO);
    if(!small) {
        LV_LOG_ERROR("failed to allocate temporary buffer");
        LV_PROFILER_DRAW_END;
        return;
    }

    /* The allocation is not initialized. A blit does not overwrite destination
     * pixels where the source is fully transparent on every backend (e.g. the
     * ThorVG emulator maps BLEND_NONE to SrcOver), so start from zero. This
     * also flushes the CPU cache before the GPU accesses the buffer. */
    lv_draw_buf_clear(small, NULL);

    vg_lite_buffer_t small_vg_buf;
    lv_vg_lite_buffer_from_draw_buf(&small_vg_buf, small);

    /* Pass 1: layer -> small
     * small = S * T(-roi) * G^-1 * physical */
    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    vg_lite_scale(scale_x, scale_y, &matrix);
    vg_lite_translate(-roi->x1, -roi->y1, &matrix);
    lv_vg_lite_matrix_multiply(&matrix, &inv_global);

    /* The layer scissor is in physical layer coordinates and must not clip
     * the temporary buffer. */
    const bool has_scissor = vg_lite_query_feature(gcFEATURE_BIT_VG_SCISSOR);
    const lv_area_t scissor_ori = u->current_scissor_area;
    if(has_scissor) {
        lv_vg_lite_disable_scissor();
    }

    /* Replace semantics: every pixel of the temporary buffer comes from the
     * source, uninitialized content must not leak in. */
    lv_vg_lite_blit(&small_vg_buf, &u->target_buffer, &matrix, VG_LITE_BLEND_NONE, 0,
                    VG_LITE_FILTER_BI_LINEAR);

    /* The temporary buffer is the source of the next operation */
    lv_vg_lite_finish(u);

    /* Restore the layer scissor. disable_scissor does not update the cached
     * area, so invalidate it to force the reprogramming. */
    if(has_scissor) {
        lv_area_set(&u->current_scissor_area, 0, 0, -1, -1);
        lv_vg_lite_set_scissor_area(u, &scissor_ori);
    }

    /* Pass 2: small -> layer, through a rounded rect over the ROI
     * physical = G * T(roi) * S^-1 * small */
    matrix = u->global_matrix;
    vg_lite_translate(roi->x1, roi->y1, &matrix);
    vg_lite_scale(1.0f / scale_x, 1.0f / scale_y, &matrix);

    /* The path is in logical coordinates, G maps it like any other draw task */
    lv_vg_lite_path_t * path = lv_vg_lite_path_get(u, VG_LITE_FP32);
    lv_vg_lite_path_set_bounding_box_area(path, &clip_area);
    lv_vg_lite_path_append_rect(path,
                                roi->x1, roi->y1,
                                width, height,
                                dsc->corner_radius);
    lv_vg_lite_path_end(path);

    vg_lite_matrix_t path_matrix = u->global_matrix;

    lv_vg_lite_draw_pattern(
        &u->target_buffer,
        lv_vg_lite_path_get_path(path),
        VG_LITE_FILL_EVEN_ODD,
        &path_matrix,
        &small_vg_buf,
        &matrix,
        VG_LITE_BLEND_NONE,
        VG_LITE_PATTERN_COLOR,
        0,
        0,
        VG_LITE_FILTER_BI_LINEAR);

    lv_vg_lite_path_drop(u, path);

    /* The GPU must be done with the temporary buffer before it is freed */
    lv_vg_lite_finish(u);
    lv_draw_buf_destroy(small);

    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int32_t get_downsample_factor(int32_t blur_radius)
{
    /**
     * One bilinear downscale/upscale round trip applies a triangular filter
     * whose effective radius equals the scale factor. Map the requested blur
     * radius directly onto the factor. Keep a floor of two because a factor
     * of one is not a blur, and a ceiling because very small intermediate
     * buffers lose shape information. An integer factor also keeps the
     * downscale aligned to whole source pixels.
     */
    return LV_CLAMP(2, blur_radius, 8);
}

#endif /*LV_USE_DRAW_VG_LITE*/
