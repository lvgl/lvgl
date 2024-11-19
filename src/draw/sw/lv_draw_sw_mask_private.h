/**
 * @file lv_draw_sw_mask_private.h
 *
 */

#ifndef LV_DRAW_SW_MASK_PRIVATE_H
#define LV_DRAW_SW_MASK_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_sw_mask.h"

#if LV_DRAW_SW_COMPLEX

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct  {
    uint8_t * buf;
    lv_opa_t * cir_opa;         /**< Opacity of values on the circumference of an 1/4 circle */
    uint16_t * x_start_on_y;    /**< The x coordinate of the circle for each y value */
    uint16_t * opa_start_on_y;  /**< The index of `cir_opa` for each y value */
    int32_t life;               /**< How many times the entry way used */
    uint32_t used_cnt;          /**< Like a semaphore to count the referencing masks */
    int32_t radius;             /**< The radius of the entry */
} lv_draw_sw_mask_radius_circle_dsc_t;

struct _lv_draw_sw_mask_common_dsc_t {
    lv_draw_sw_mask_xcb_t cb;
    lv_draw_sw_mask_type_t type;
};

struct _lv_draw_sw_mask_line_param_t {
    /** The first element must be the common descriptor */
    lv_draw_sw_mask_common_dsc_t dsc;

    struct {
        /*First point*/
        lv_point_t p1;

        /*Second point*/
        lv_point_t p2;

        /*Which side to keep?*/
        lv_draw_sw_mask_line_side_t side : 3;
    } cfg;

    /** A point of the line */
    lv_point_t origo;

    /** X / (1024*Y) steepness (X is 0..1023 range). What is the change of X in 1024 Y? */
    int32_t xy_steep;

    /** Y / (1024*X) steepness (Y is 0..1023 range). What is the change of Y in 1024 X? */
    int32_t yx_steep;

    /** Helper which stores yx_steep for flat lines and xy_steep for steep (non flat) lines */
    int32_t steep;

    /** Steepness in 1 px in 0..255 range. Used only by flat lines. */
    int32_t spx;

    /** 1: It's a flat line? (Near to horizontal) */
    uint8_t flat : 1;

    /** Invert the mask. The default is: Keep the left part.
     *It is used to select left/right/top/bottom */
    uint8_t inv: 1;
};

struct _lv_draw_sw_mask_angle_param_t {
    /** The first element must be the common descriptor */
    lv_draw_sw_mask_common_dsc_t dsc;

    struct {
        lv_point_t vertex_p;
        int32_t start_angle;
        int32_t end_angle;
    } cfg;

    lv_draw_sw_mask_line_param_t start_line;
    lv_draw_sw_mask_line_param_t end_line;
    uint16_t delta_deg;
};

struct _lv_draw_sw_mask_radius_param_t {
    /** The first element must be the common descriptor */
    lv_draw_sw_mask_common_dsc_t dsc;

    struct {
        lv_area_t rect;
        int32_t radius;
        /** Invert the mask. 0: Keep the pixels inside. */
        uint8_t outer: 1;
    } cfg;

    lv_draw_sw_mask_radius_circle_dsc_t * circle;
};

struct _lv_draw_sw_mask_fade_param_t {
    /** The first element must be the common descriptor */
    lv_draw_sw_mask_common_dsc_t dsc;

    struct {
        lv_area_t coords;
        int32_t y_top;
        int32_t y_bottom;
        lv_opa_t opa_top;
        lv_opa_t opa_bottom;
    } cfg;

};

struct _lv_draw_sw_mask_map_param_t {
    /** The first element must be the common descriptor */
    lv_draw_sw_mask_common_dsc_t dsc;

    struct {
        lv_area_t coords;
        const lv_opa_t * map;
    } cfg;
};

typedef lv_draw_sw_mask_radius_circle_dsc_t lv_draw_sw_mask_radius_circle_dsc_arr_t[LV_DRAW_SW_CIRCLE_CACHE_SIZE];

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Called by LVGL the rendering of a screen is ready to clean up
 * the temporal (cache) data of the masks
 */
void lv_draw_sw_mask_cleanup(void);

/**********************
 *      MACROS
 **********************/

#endif  /*LV_DRAW_SW_COMPLEX*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_MASK_PRIVATE_H*/
