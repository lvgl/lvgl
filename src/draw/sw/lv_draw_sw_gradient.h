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

#if LV_USE_DRAW_SW

/*********************
 *      DEFINES
 *********************/
#if LV_GRADIENT_MAX_STOPS < 2
#error LVGL needs at least 2 stops for gradients. Please increase the LV_GRADIENT_MAX_STOPS
#endif

#define LV_GRAD_LEFT    LV_PCT(0)
#define LV_GRAD_RIGHT   LV_PCT(100)
#define LV_GRAD_TOP     LV_PCT(0)
#define LV_GRAD_BOTTOM  LV_PCT(100)
#define LV_GRAD_CENTER  LV_PCT(50)

/**********************
 *      TYPEDEFS
 **********************/
typedef lv_color_t lv_grad_color_t;

/**********************
 *      PROTOTYPES
 **********************/
/** Compute the color in the given gradient and fraction
 *  Gradient are specified in a virtual [0-255] range, so this function scales the virtual range to the given range
 * @param dsc       The gradient descriptor to use
 * @param range     The range to use in computation.
 * @param frac      The current part used in the range. frac is in [0; range]
 * @param color_out Calculated gradient color
 * @param opa_out   Calculated opacity
 */

void /* LV_ATTRIBUTE_FAST_MEM */ lv_gradient_color_calculate(const lv_grad_dsc_t * dsc, int32_t range,
                                                             int32_t frac, lv_grad_color_t * color_out, lv_opa_t * opa_out);

/** Get a gradient cache from the given parameters */
lv_grad_t * lv_gradient_get(const lv_grad_dsc_t * gradient, int32_t w, int32_t h);

/**
 * Clean up the gradient item after it was get with `lv_grad_get_from_cache`.
 * @param grad      pointer to a gradient
 */
void lv_gradient_cleanup(lv_grad_t * grad);

/**
 * Initialize gradient color map from a table
 * @param grad      pointer to a gradient descriptor
 * @param colors    color array
 * @param fracs     position array (0..255): if NULL, then colors are distributed evenly
 * @param opa       opacity array: if NULL, then LV_OPA_COVER is assumed
 * @param num_stops number of gradient stops (1..LV_GRADIENT_MAX_STOPS)
 */
void lv_gradient_init_stops(lv_grad_dsc_t * grad, const lv_color_t colors[], const lv_opa_t opa[],
                            const uint8_t fracs[], int num_stops);

#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS

/**
 * Helper function to initialize linear gradient
 * @param dsc      gradient descriptor
 * @param from_x   start x position: can be a coordinate or an lv_pct() value
 *                 predefined constants LV_GRAD_LEFT, LV_GRAD_RIGHT, LV_GRAD_TOP, LV_GRAD_BOTTOM, LV_GRAD_CENTER can be used as well
 * @param from_y   start y position
 * @param to_x     end x position
 * @param to_y     end y position
 * @param extend   one of LV_GRAD_EXTEND_PAD, LV_GRAD_EXTEND_REPEAT or LV_GRAD_EXTEND_REFLECT
 */
void lv_grad_linear_init(lv_grad_dsc_t * dsc, int32_t from_x, int32_t from_y, int32_t to_x, int32_t to_y,
                         lv_grad_extend_t extend);

/**
 * Helper function to initialize radial gradient
 * @param dsc      gradient descriptor
 * @param center_x center x position: can be a coordinate or an lv_pct() value
 *                 predefined constants LV_GRAD_LEFT, LV_GRAD_RIGHT, LV_GRAD_TOP, LV_GRAD_BOTTOM, LV_GRAD_CENTER can be used as well
 * @param center_y center y position
 * @param to_x     point on the end circle x position
 * @param to_y     point on the end circle y position
 * @param extend   one of LV_GRAD_EXTEND_PAD, LV_GRAD_EXTEND_REPEAT or LV_GRAD_EXTEND_REFLECT
 */
void lv_grad_radial_init(lv_grad_dsc_t * dsc, int32_t center_x, int32_t center_y, int32_t to_x, int32_t to_y,
                         lv_grad_extend_t extend);

/**
 * Set focal (starting) circle of a radial gradient
 * @param dsc      gradient descriptor
 * @param center_x center x position: can be a coordinate or an lv_pct() value
 *                 predefined constants LV_GRAD_LEFT, LV_GRAD_RIGHT, LV_GRAD_TOP, LV_GRAD_BOTTOM, LV_GRAD_CENTER can be used as well
 * @param center_y center y position
 * @param radius   radius of the starting circle (NOTE: this must be a scalar number, not percentage)
 */
void lv_grad_radial_set_focal(lv_grad_dsc_t * dsc, int32_t center_x, int32_t center_y, int32_t radius);

/**
 * Helper function to initialize conical gradient
 * @param dsc      gradient descriptor
 * @param center_x center x position: can be a coordinate or an lv_pct() value
 *                 predefined constants LV_GRAD_LEFT, LV_GRAD_RIGHT, LV_GRAD_TOP, LV_GRAD_BOTTOM, LV_GRAD_CENTER can be used as well
 * @param center_y center y position
 * @param start_angle   start angle in degrees
 * @param end_angle     end angle in degrees
 * @param extend   one of LV_GRAD_EXTEND_PAD, LV_GRAD_EXTEND_REPEAT or LV_GRAD_EXTEND_REFLECT
 */
void lv_grad_conical_init(lv_grad_dsc_t * dsc, int32_t center_x, int32_t center_y, int32_t start_angle,
                          int32_t end_angle, lv_grad_extend_t extend);

/**
 * Calculate constants from the given parameters that are used during rendering
 * @param dsc      gradient descriptor
 */
void lv_gradient_linear_setup(lv_grad_dsc_t * dsc, const lv_area_t * coords);

/**
 * Free up the allocated memory for the gradient calculation
 * @param dsc      gradient descriptor
 */
void lv_gradient_linear_cleanup(lv_grad_dsc_t * dsc);

/**
 * Calculate a line segment of a linear gradient
 * @param dsc       gradient descriptor
 * @param xp        starting point x coordinate in gradient space
 * @param yp        starting point y coordinate in gradient space
 * @param width     width of the line segment in pixels
 * @param result    color buffer for the resulting line segment
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_gradient_linear_get_line(lv_grad_dsc_t * dsc, int32_t xp, int32_t yp, int32_t width,
                                                             lv_grad_t * result);

/**
 * Calculate constants from the given parameters that are used during rendering
 * @param dsc      gradient descriptor
 */
void lv_gradient_radial_setup(lv_grad_dsc_t * dsc, const lv_area_t * coords);

/**
 * Free up the allocated memory for the gradient calculation
 * @param dsc      gradient descriptor
 */
void lv_gradient_radial_cleanup(lv_grad_dsc_t * dsc);

/**
 * Calculate a line segment of a radial gradient
 * @param dsc       gradient descriptor
 * @param xp        starting point x coordinate in gradient space
 * @param yp        starting point y coordinate in gradient space
 * @param width     width of the line segment in pixels
 * @param result    color buffer for the resulting line segment
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_gradient_radial_get_line(lv_grad_dsc_t * dsc, int32_t xp, int32_t yp, int32_t width,
                                                             lv_grad_t * result);

/**
 * Calculate constants from the given parameters that are used during rendering
 * @param dsc      gradient descriptor
 */
void lv_gradient_conical_setup(lv_grad_dsc_t * dsc, const lv_area_t * coords);

/**
 * Free up the allocated memory for the gradient calculation
 * @param dsc      gradient descriptor
 */
void lv_gradient_conical_cleanup(lv_grad_dsc_t * dsc);

/**
 * Calculate a line segment of a conical gradient
 * @param dsc       gradient descriptor
 * @param xp        starting point x coordinate in gradient space
 * @param yp        starting point y coordinate in gradient space
 * @param width     width of the line segment in pixels
 * @param result    color buffer for the resulting line segment
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_gradient_conical_get_line(lv_grad_dsc_t * dsc, int32_t xp, int32_t yp,
                                                              int32_t width,
                                                              lv_grad_t * result);

#endif /*LV_USE_DRAW_SW_COMPLEX_GRADIENTS*/

#endif /*LV_USE_DRAW_SW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_GRADIENT_H*/
