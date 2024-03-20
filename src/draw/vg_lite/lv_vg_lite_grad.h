/**
 * @file lv_vg_lite_grad.h
 *
 */

#ifndef LV_VG_LITE_GRAD_H
#define LV_VG_LITE_GRAD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_vg_lite_utils.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_vg_lite_grad_init(
    struct _lv_draw_vg_lite_unit_t * u,
    uint32_t linear_grad_cache_cnt,
    uint32_t radial_grad_cache_cnt);

void lv_vg_lite_grad_deinit(struct _lv_draw_vg_lite_unit_t * u);

void lv_vg_lite_grad_area_to_matrix(vg_lite_matrix_t * grad_matrix, const lv_area_t * area, lv_grad_dir_t dir);

void lv_vg_lite_draw_linear_grad(
    struct _lv_draw_vg_lite_unit_t * u,
    vg_lite_buffer_t * buffer,
    vg_lite_path_t * path,
    const lv_grad_dsc_t * grad,
    const vg_lite_matrix_t * grad_matrix,
    const vg_lite_matrix_t * matrix,
    vg_lite_fill_t fill,
    vg_lite_blend_t blend);

#if LV_USE_VECTOR_GRAPHIC

void lv_vg_lite_draw_radial_grad(
    struct _lv_draw_vg_lite_unit_t * u,
    vg_lite_buffer_t * buffer,
    vg_lite_path_t * path,
    const lv_vector_gradient_t * grad,
    const vg_lite_matrix_t * grad_matrix,
    const vg_lite_matrix_t * matrix,
    vg_lite_fill_t fill,
    vg_lite_blend_t blend);

#endif /* LV_USE_VECTOR_GRAPHIC */

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_VG_LITE_GRAD_H*/
