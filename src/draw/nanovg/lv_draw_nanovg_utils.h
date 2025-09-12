/**
 * @file lv_draw_nanovg_utils.h
 *
 */

#ifndef LV_DRAW_NANOVG_UTILS_H
#define LV_DRAW_NANOVG_UTILS_H

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

void lv_nanovg_transform(NVGcontext * ctx, const lv_matrix_t * matrix);

void lv_nanovg_path_append_rect(NVGcontext * ctx, float x, float y, float w, float h, float r);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRAW_NANOVG */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_NANOVG_UTILS_H*/
