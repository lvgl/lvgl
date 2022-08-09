/**
 * @file lv_draw_sw_dither.h
 *
 */

#ifndef LV_DRAW_SW_DITHER_H
#define LV_DRAW_SW_DITHER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj_pos.h"

#if LV_USE_DRAW_SW

/*********************
 *      DEFINES
 *********************/
#if LV_COLOR_DEPTH < 32 && LV_DRAW_SW_GRADIENT_DITHER == 1
#define _DITHER_GRADIENT 1
#else
#define _DITHER_GRADIENT 0
#endif

/**********************
 *      TYPEDEFS
 **********************/
#if _DITHER_GRADIENT
/*A signed error color component*/
typedef struct {
    int8_t r, g, b;
} lv_scolor24_t;

struct _lv_gradient_cache_t;
typedef void (*lv_dither_func_t)(struct _lv_gradient_cache_t * grad, lv_coord_t x, lv_coord_t y, lv_coord_t w);

#endif


/**********************
 *    PROTOTYPES
 **********************/
#if _DITHER_GRADIENT
LV_ATTRIBUTE_FAST_MEM void lv_dither_none(struct _lv_gradient_cache_t * grad, lv_coord_t x, lv_coord_t y, lv_coord_t w);

LV_ATTRIBUTE_FAST_MEM void lv_dither_ordered_hor(struct _lv_gradient_cache_t * grad, const lv_coord_t xs,
                                                 const lv_coord_t y, const lv_coord_t w);
LV_ATTRIBUTE_FAST_MEM void lv_dither_ordered_ver(struct _lv_gradient_cache_t * grad, const lv_coord_t xs,
                                                 const lv_coord_t y, const lv_coord_t w);

#if LV_DRAW_SW_GRADIENT_DITHER_ERROR_DIFFUSION == 1
LV_ATTRIBUTE_FAST_MEM void lv_dither_err_diff_hor(struct _lv_gradient_cache_t * grad, const lv_coord_t xs,
                                                  const lv_coord_t y, const lv_coord_t w);
LV_ATTRIBUTE_FAST_MEM void lv_dither_err_diff_ver(struct _lv_gradient_cache_t * grad, const lv_coord_t xs,
                                                  const lv_coord_t y, const lv_coord_t w);
#endif /* LV_DRAW_SW_GRADIENT_DITHER_ERROR_DIFFUSION */

#endif /* _DITHER_GRADIENT */
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
