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

typedef struct {
    lv_color_t * color_map;
    lv_opa_t * opa_map;
    uint32_t size;
} lv_grad_t;

#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS

typedef struct {
    /* w = (-b(xp, yp) + sqrt(sqr(b(xp, yp)) - 4 * a * c(xp, yp))) / (2 * a) */
    int32_t x0;         /* center of the start circle */
    int32_t y0;         /* center of the start circle */
    int32_t r0;         /* radius of the start circle */
    int32_t inv_dr;     /* 1 / (r1 - r0) */
    int32_t a4;         /* 4 * a */
    int32_t inv_a4;     /* 1 / (4 * a) */
    int32_t dx;
    /* b(xp, yp) = xp * bpx + yp * bpy + bc */
    int32_t bpx;
    int32_t bpy;
    int32_t bc;
    lv_area_t clip_area;
    lv_grad_t * cgrad;  /*256 element cache buffer containing the gradient color map*/
} lv_grad_radial_state_t;

typedef struct {
    /* w = a * xp + b * yp + c */
    int32_t a;
    int32_t b;
    int32_t c;
    lv_grad_t * cgrad; /*256 element cache buffer containing the gradient color map*/
} lv_grad_linear_state_t;

typedef struct {
    /* w = a * xp + b * yp + c */
    int32_t x0;
    int32_t y0;
    int32_t a;
    int32_t da;
    int32_t inv_da;
    lv_grad_t * cgrad; /*256 element cache buffer containing the gradient color map*/
} lv_grad_conical_state_t;


#endif

/**********************
 *      PROTOTYPES
 **********************/


/**
 * Calculate the colors and opacities for a horizontal or vertical gradient
 * @param dsc      gradient descriptor
 * @param w        width of the gradient
 * @param h        height of the gradient
 */
lv_grad_t * lv_draw_sw_grad_get(const lv_grad_dsc_t * dsc, int32_t w, int32_t h);

/**
 * Clean up the gradient item after it was get with `lv_grad_get_from_cache`.
 * @param grad      pointer to a gradient
 */
void lv_draw_sw_grad_delete(lv_grad_t * grad);

#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS

/**
 * Calculate constants from the given parameters that are used during rendering
 * @param dsc      gradient descriptor
 * @param coords   area of the gradient
 * @return         state for the gradient calculation (needs to be freed by lv_gradient_linear_cleanup)
 */
lv_grad_linear_state_t * lv_draw_sw_grad_linear_create(const lv_grad_dsc_t * dsc, const lv_area_t * coords);

/**
 * Free up the allocated memory for the gradient calculation
 * @param state     the state, initialized by lv_gradient_linear_setup
 */
void lv_gradient_linear_delete(lv_grad_linear_state_t * state);

/**
 * Calculate a line segment of a linear gradient
 * @param dsc       gradient descriptor
 * @param state     the state, initialized by lv_gradient_linear_setup
 * @param xp        starting point x coordinate in gradient space
 * @param yp        starting point y coordinate in gradient space
 * @param width     width of the line segment in pixels
 * @param result    color buffer for the resulting line segment
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_gradient_linear_get_line(const lv_grad_dsc_t * dsc, lv_grad_linear_state_t * state,
                                                             int32_t xp, int32_t yp, int32_t width,
                                                             lv_grad_t * result);

/**
 * Calculate constants from the given parameters that are used during rendering
 * @param dsc      gradient descriptor
 * @param coords   area of the gradient
 * @return         state for the gradient calculation (needs to be freed by lv_gradient_radial_cleanup)
 */
lv_grad_radial_state_t * lv_draw_sw_grad_radial_create(const lv_grad_dsc_t * dsc, const lv_area_t * coords);

/**
 * Free up the allocated memory for the gradient calculation
 * @param state     the state, initialized by lv_gradient_linear_setup
 */
void lv_draw_sw_grad_radial_delete(lv_grad_radial_state_t * state);

/**
 * Calculate a line segment of a radial gradient
 * @param dsc       gradient descriptor
 * @param xp        starting point x coordinate in gradient space
 * @param yp        starting point y coordinate in gradient space
 * @param width     width of the line segment in pixels
 * @param result    color buffer for the resulting line segment
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_sw_grad_radial_get_line(const lv_grad_dsc_t * dsc,
                                                                 lv_grad_radial_state_t * state, int32_t xp, int32_t yp, int32_t width,
                                                                 lv_grad_t * result);

/**
 * Calculate constants from the given parameters that are used during rendering
 * @param dsc      gradient descriptor
 */
lv_grad_conical_state_t * lv_draw_sw_grad_conical_create(const lv_grad_dsc_t * dsc, const lv_area_t * coords);

/**
 * Free up the allocated memory for the gradient calculation
 * @param dsc      gradient descriptor
 */
void lv_draw_sw_grad_conical_delete(lv_grad_conical_state_t * state);

/**
 * Calculate a line segment of a conical gradient
 * @param dsc       gradient descriptor
 * @param xp        starting point x coordinate in gradient space
 * @param yp        starting point y coordinate in gradient space
 * @param width     width of the line segment in pixels
 * @param result    color buffer for the resulting line segment
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_sw_grad_conical_get_line(const lv_grad_dsc_t * dsc,
                                                                  lv_grad_conical_state_t * state, int32_t xp, int32_t yp,
                                                                  int32_t width,
                                                                  lv_grad_t * result);

#endif /*LV_USE_DRAW_SW_COMPLEX_GRADIENTS*/

#endif /*LV_USE_DRAW_SW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_GRADIENT_H*/
