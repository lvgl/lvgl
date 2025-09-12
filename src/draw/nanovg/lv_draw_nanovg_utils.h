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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

static inline void lv_nanovg_matrix_to_transform(float * t, const lv_matrix_t * m)
{
    LV_ASSERT_NULL(t);
    LV_ASSERT_NULL(m);
    t[0] = m->m[0][0];
    t[1] = m->m[1][0];
    t[2] = m->m[0][1];
    t[3] = m->m[1][1];
    t[4] = m->m[0][2];
    t[5] = m->m[1][2];
}

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRAW_NANOVG */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_NANOVG_UTILS_H*/
