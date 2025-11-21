/**
 * @file lv_polar_chart_private.h
 *
 */

#ifndef LV_POLAR_CHART_PRIVATE_H
#define LV_POLAR_CHART_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "lv_polar_chart.h"

#if LV_USE_POLAR_CHART != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Descriptor a chart series
 */
struct _lv_polar_chart_series_t {
    int32_t * angle_points;
    int32_t * radial_points;
    lv_color_t color;
    uint32_t start_point;
    uint32_t hidden : 1;
    uint32_t angle_ext_buf_assigned : 1;
    uint32_t radial_ext_buf_assigned : 1;
};

struct _lv_polar_chart_cursor_t {
    lv_point_t pos;
    int32_t point_id;
    lv_color_t color;
    lv_polar_chart_series_t * ser;
    lv_dir_t dir;
    uint32_t pos_set: 1;        /**< 1: pos is set; 0: point_id is set */
};

struct _lv_polar_chart_t {
    lv_obj_t obj;
    lv_ll_t series_ll;          /**< Linked list for series (stores lv_polar_chart_series_t) */
    lv_ll_t cursor_ll;          /**< Linked list for cursors (stores lv_polar_chart_cursor_t) */
    int32_t radial_min;
    int32_t radial_max;
    int32_t angle_min;
    int32_t angle_max;
    int32_t pressed_point_id;
    uint32_t angle_div_cnt;          /**< Number of angle division lines */
    uint32_t radial_div_cnt;          /**< Number of radial division lines */
    uint32_t point_cnt;         /**< Number of points in all series */
    lv_polar_chart_type_t type  : 4;  /**< Chart type */
    lv_polar_chart_update_mode_t update_mode : 2;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_POLAR_CHART != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_POLAR_CHART_PRIVATE_H*/
