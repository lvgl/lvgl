/**
 * @file lv_grad.h
 *
 */

#ifndef LV_GRAD_H
#define LV_GRAD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "lv_color.h"
#include "lv_area.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * The direction of the gradient.
 */
typedef enum {
    LV_GRAD_DIR_NONE,       /**< No gradient (the `grad_color` property is ignored)*/
    LV_GRAD_DIR_VER,        /**< Simple vertical (top to bottom) gradient*/
    LV_GRAD_DIR_HOR,        /**< Simple horizontal (left to right) gradient*/
    LV_GRAD_DIR_LINEAR,     /**< Linear gradient defined by start and end points. Can be at any angle.*/
    LV_GRAD_DIR_RADIAL,     /**< Radial gradient defined by start and end circles*/
    LV_GRAD_DIR_CONICAL,    /**< Conical gradient defined by center point, start and end angles*/
} lv_grad_dir_t;

/**
 * Gradient behavior outside the defined range.
 */
typedef enum {
    LV_GRAD_EXTEND_PAD,     /**< Repeat the same color*/
    LV_GRAD_EXTEND_REPEAT,  /**< Repeat the pattern*/
    LV_GRAD_EXTEND_REFLECT, /**< Repeat the pattern mirrored*/
} lv_grad_extend_t;

/** A gradient stop definition.
 *  This matches a color and a position in a virtual 0-255 scale.
 */
typedef struct {
    lv_color_t color;   /**< The stop color */
    lv_opa_t   opa;     /**< The opacity of the color*/
    uint8_t    frac;    /**< The stop position in 1/255 unit */
} lv_grad_stop_t;

/** A descriptor of a gradient. */
typedef struct {
    lv_grad_stop_t   stops[LV_GRADIENT_MAX_STOPS];  /**< A gradient stop array */
    uint8_t          stops_count;                   /**< The number of used stops in the array */
    lv_grad_dir_t    dir : 4;                       /**< The gradient direction.
                                                         * Any of LV_GRAD_DIR_NONE, LV_GRAD_DIR_VER, LV_GRAD_DIR_HOR,
                                                         * LV_GRAD_TYPE_LINEAR, LV_GRAD_TYPE_RADIAL, LV_GRAD_TYPE_CONICAL */
    lv_grad_extend_t     extend : 3;                    /**< Behaviour outside the defined range.
                                                         * LV_GRAD_EXTEND_NONE, LV_GRAD_EXTEND_PAD, LV_GRAD_EXTEND_REPEAT, LV_GRAD_EXTEND_REFLECT */
#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS
    union {
        /*Linear gradient parameters*/
        struct {
            lv_point_t  start;                          /**< Linear gradient vector start point */
            lv_point_t  end;                            /**< Linear gradient vector end point */
        } linear;
        /*Radial gradient parameters*/
        struct {
            lv_point_t  focal;                          /**< Center of the focal (starting) circle in local coordinates */
            /* (can be the same as the ending circle to create concentric circles) */
            lv_point_t  focal_extent;                   /**< Point on the circle (can be the same as the center) */
            lv_point_t  end;                            /**< Center of the ending circle in local coordinates */
            lv_point_t  end_extent;                     /**< Point on the circle determining the radius of the gradient */
        } radial;
        /*Conical gradient parameters*/
        struct {
            lv_point_t  center;                         /**< Conical gradient center point */
            int16_t     start_angle;                    /**< Start angle 0..3600 */
            int16_t     end_angle;                      /**< End angle 0..3600 */
        } conical;
    } params;
    void * state;
#endif
} lv_grad_dsc_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize gradient color map from a table
 * @param grad      pointer to a gradient descriptor
 * @param colors    color array
 * @param fracs     position array (0..255): if NULL, then colors are distributed evenly
 * @param opa       opacity array: if NULL, then LV_OPA_COVER is assumed
 * @param num_stops number of gradient stops (1..LV_GRADIENT_MAX_STOPS)
 */
void lv_grad_init_stops(lv_grad_dsc_t * grad, const lv_color_t colors[], const lv_opa_t opa[],
                        const uint8_t fracs[], int num_stops);

/**
 * Helper function to initialize a horizontal gradient.
 * @param dsc      gradient descriptor
 */
void lv_grad_horizontal_init(lv_grad_dsc_t * dsc);

/**
 * Helper function to initialize a vertical gradient.
 * @param dsc      gradient descriptor
 */
void lv_grad_vertical_init(lv_grad_dsc_t * dsc);

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

#endif /*LV_USE_DRAW_SW_COMPLEX_GRADIENTS*/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GRAD_H*/
