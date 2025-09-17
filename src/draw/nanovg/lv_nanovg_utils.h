/**
 * @file lv_nanovg_utils.h
 *
 */

#ifndef LV_NANOVG_UTILS_H
#define LV_NANOVG_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_NANOVG

#include "../../misc/lv_assert.h"
#include "../../misc/lv_matrix.h"
#include "../../misc/lv_color.h"
#include "../../libs/nanovg/nanovg.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_nanovg_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_nanovg_utils_init(struct _lv_draw_nanovg_unit_t * u);

void lv_nanovg_utils_deinit(struct _lv_draw_nanovg_unit_t * u);

static inline void lv_nanovg_matrix_convert(float * xform, const lv_matrix_t * matrix)
{
    LV_ASSERT_NULL(xform);
    LV_ASSERT_NULL(matrix);
    xform[0] = matrix->m[0][0];
    xform[1] = matrix->m[1][0];
    xform[2] = matrix->m[0][1];
    xform[3] = matrix->m[1][1];
    xform[4] = matrix->m[0][2];
    xform[5] = matrix->m[1][2];
}

static inline NVGcolor lv_nanovg_color_convert(lv_color_t color, lv_opa_t opa)
{
    return nvgRGBA(color.red, color.green, color.blue, opa);
}

void lv_nanovg_transform(NVGcontext * ctx, const lv_matrix_t * matrix);

void lv_nanovg_set_clip_area(NVGcontext * ctx, const lv_area_t * area);

void lv_nanovg_path_append_rect(NVGcontext * ctx, float x, float y, float w, float h, float r);

void lv_nanovg_path_append_arc_right_angle(NVGcontext * ctx,
                                           float start_x, float start_y,
                                           float center_x, float center_y,
                                           float end_x, float end_y);

void lv_nanovg_path_append_arc(NVGcontext * ctx,
                               float cx, float cy,
                               float radius,
                               float start_angle,
                               float sweep,
                               bool pie);

void lv_nanovg_fill(NVGcontext * ctx, enum NVGwinding winding, enum NVGcompositeOperation composite_operation,
                    NVGcolor color);

void lv_nanovg_end_frame(struct _lv_draw_nanovg_unit_t * u);

void lv_nanovg_convert_a8_to_nvgcolor(lv_draw_buf_t * dest, const lv_draw_buf_t * src, const lv_color32_t color);

lv_draw_buf_t * lv_nanovg_reshape_global_image(struct _lv_draw_nanovg_unit_t * u,
                                               lv_color_format_t cf,
                                               uint32_t w,
                                               uint32_t h);

int lv_nanovg_push_image(struct _lv_draw_nanovg_unit_t * u, const lv_draw_buf_t * src_buf, lv_color32_t color);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRAW_NANOVG */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_NANOVG_UTILS_H*/
