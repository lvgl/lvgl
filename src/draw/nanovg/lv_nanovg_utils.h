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

typedef struct {
    float xform[6];
} lv_nanovg_matrixt_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

static inline void lv_nanovg_matrix_convert(lv_nanovg_matrixt_t * dest, const lv_matrix_t * src)
{
    LV_ASSERT_NULL(dest);
    LV_ASSERT_NULL(src);
    dest->xform[0] = src->m[0][0];
    dest->xform[1] = src->m[1][0];
    dest->xform[2] = src->m[0][1];
    dest->xform[3] = src->m[1][1];
    dest->xform[4] = src->m[0][2];
    dest->xform[5] = src->m[1][2];
}

static inline NVGcolor lv_nanovg_color_convert(lv_color_t color, lv_opa_t opa, bool pre_mul)
{
    if(pre_mul && opa < LV_OPA_COVER) {
        color.red = LV_UDIV255(color.red * opa);
        color.green = LV_UDIV255(color.green * opa);
        color.blue = LV_UDIV255(color.blue * opa);
    }

    return nvgRGBA(color.red, color.green, color.blue, opa);
}

void lv_nanovg_transform(NVGcontext * ctx, const lv_matrix_t * matrix);

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

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRAW_NANOVG */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_NANOVG_UTILS_H*/
