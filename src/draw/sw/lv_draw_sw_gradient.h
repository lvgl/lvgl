/**
 * @file lv_draw_sw_gradient.h
 *
 */

#ifndef LV_DRAW_SW_GRADIENT_H
#define LV_DRAW_SW_GRADIENT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_color.h"
#include "../../misc/lv_style.h"
#include "lv_draw_sw_dither.h"

/*********************
 *      DEFINES
 *********************/
#if LV_GRADIENT_MAX_STOPS < 2
#error LVGL needs at least 2 stops for gradients. Please increase the LV_GRADIENT_MAX_STOPS
#endif


/**********************
 *      TYPEDEFS
 **********************/
#if _DITHER_GRADIENT
typedef lv_color32_t lv_grad_color_t;
#else
typedef lv_color_t lv_grad_color_t;
#endif

/** To avoid recomputing gradient for each draw operation,
 *  it's possible to cache the computation in this structure instance.
 *  Whenever possible, this structure is reused instead of recomputing the gradient map */
typedef struct _lv_gradient_cache_t {
    uint32_t        key;          /**< A discriminating key that's built from the drawing operation.
                                   * If the key does not match, the cache item is not used */
    uint32_t        life : 31;    /**< A life counter that's incremented on usage. Higher counter is
                                   * less likely to be evicted from the cache */
    uint32_t        filled : 1;   /**< Used to skip dithering in it if already done */
    lv_color_t   *  map;          /**< The computed gradient low bitdepth color map, points into the
                                   * cache's buffer, no free needed */
    lv_coord_t      size;         /**< The computed gradient color map size, in colors */
#if _DITHER_GRADIENT
    lv_color32_t  * hmap;         /**< If dithering, we need to store the current, high bitdepth gradient
                                   * map too, points to the cache's buffer, no free needed */
    lv_coord_t      hmap_size;    /**< The array size in pixels */
#if LV_DITHER_ERROR_DIFFUSION == 1
    lv_scolor24_t * error_acc;    /**< Error diffusion dithering algorithm requires storing the last error
                                   * drawn, points to the cache's buffer, no free needed  */
#endif
#endif
} lv_gradient_cache_t;


/**********************
 *      PROTOTYPES
 **********************/
/** Compute the color in the given gradient and fraction
 *  Gradient are specified in a virtual [0-255] range, so this function scales the virtual range to the given range
 * @param dsc       The gradient descriptor to use
 * @param range     The range to use in computation.
 * @param frac      The current part used in the range. frac is in [0; range]
 */
LV_ATTRIBUTE_FAST_MEM lv_grad_color_t lv_grad_get(const lv_gradient_t * dsc, lv_coord_t range, lv_coord_t frac);

/** Set the gradient cache size */
void lv_grad_set_cache_size(size_t max_bytes);

/** Get a gradient cache from the given parameters */
lv_gradient_cache_t * lv_grad_get_from_cache(const lv_gradient_t * gradient, lv_coord_t w, lv_coord_t h);

/** Evict item from the gradient cache (not used anymore).
 *  This bypass the life counter on the item to remove this item.
 */
void lv_grad_pop_from_cache(const lv_gradient_t * gradient, lv_coord_t w, lv_coord_t h);

/** Free the gradient cache */
void lv_grad_free_cache(void);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_GRADIENT_H*/
