/**
 * @file lv_vglite_grad.h
 *
 */

#ifndef LV_VGLITE_GRAD_H
#define LV_VGLITE_GRAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../lv_conf_internal.h"

#if LV_USE_DRAW_VGLITE
#if LV_VGLITE_VECTOR && LV_USE_VECTOR_GRAPHIC

#if LV_VGLITE_VECTOR
#error "VGLITE Vector support is currently not available, please disable this option in lv_conf.h"
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lvgl.h"

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

/**
 * @brief Create a gradient context
 * @param cache_cnt number of cache entries
 * @param unit the draw unit
 */
struct _lv_vg_lite_grad_ctx_t * lv_vg_lite_grad_ctx_create(uint32_t cache_cnt, struct _lv_draw_vg_lite_unit_t * unit);

/**
 * @brief Delete a gradient context
 * @param ctx the gradient context to delete
 */
void lv_vg_lite_grad_ctx_delete(struct _lv_vg_lite_grad_ctx_t * ctx);

/**
 * @brief Get the pending list of gradient items
 * @param ctx the gradient context
 */
struct _lv_vg_lite_pending_t * lv_vg_lite_grad_ctx_get_pending(struct _lv_vg_lite_grad_ctx_t * ctx);

/**
 * @brief Draw a gradient
 * @param ctx the gradient context
 * @param buffer the target buffer
 * @param path the path to draw the gradient on
 * @param grad the gradient descriptor
 * @param grad_matrix the gradient matrix
 * @param matrix the matrix to apply to the gradient
 * @param fill the fill rule
 * @param blend the blend mode
 * @return true: success, false: failed
 */
bool lv_vg_lite_draw_grad(
    struct _lv_vg_lite_grad_ctx_t * ctx,
    vg_lite_buffer_t * buffer,
    vg_lite_path_t * path,
    const lv_vector_gradient_t * grad,
    const vg_lite_matrix_t * grad_matrix,
    const vg_lite_matrix_t * matrix,
    vg_lite_fill_t fill,
    vg_lite_blend_t blend);

/**
 * @brief Draw a gradient helper
 * @param ctx the gradient context
 * @param buffer the target buffer
 * @param path the path to draw the gradient on
 * @param area the area to draw the gradient on
 * @param grad_dsc the gradient descriptor
 * @param matrix the matrix to apply to the gradient
 * @param fill the fill rule
 * @param blend the blend mode
 * @return true: success, false: failed
 */
bool lv_vg_lite_draw_grad_helper(
    struct _lv_vg_lite_grad_ctx_t * ctx,
    vg_lite_buffer_t * buffer,
    vg_lite_path_t * path,
    const lv_area_t * area,
    const lv_grad_dsc_t * grad_dsc,
    const vg_lite_matrix_t * matrix,
    vg_lite_fill_t fill,
    vg_lite_blend_t blend);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE && LV_USE_VECTOR_GRAPHIC*/
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_VGLITE_GRAD_H*/
