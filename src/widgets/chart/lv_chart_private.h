/**
 * @file lv_chart_private.h
 *
 */

#ifndef LV_CHART_PRIVATE_H
#define LV_CHART_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "lv_chart.h"

#if LV_USE_CHART != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Descriptor a chart series
 */
struct _lv_chart_series_t {
    int32_t * x_points;
    int32_t * y_points;
    lv_color_t color;
    uint32_t start_point;
    uint32_t hidden : 1;
    uint32_t x_ext_buf_assigned : 1;
    uint32_t y_ext_buf_assigned : 1;
    uint32_t x_axis_sec : 1;
    uint32_t y_axis_sec : 1;
};

struct _lv_chart_cursor_t {
    lv_point_t pos;
    int32_t point_id;
    lv_color_t color;
    lv_chart_series_t * ser;
    lv_dir_t dir;
    uint32_t pos_set: 1;        /**< 1: pos is set; 0: point_id is set */
};

struct _lv_chart_t {
    lv_obj_t obj;
    lv_ll_t series_ll;          /**< Linked list for series (stores lv_chart_series_t) */
    lv_ll_t cursor_ll;          /**< Linked list for cursors (stores lv_chart_cursor_t) */
    int32_t ymin[2];
    int32_t ymax[2];
    int32_t xmin[2];
    int32_t xmax[2];
    int32_t pressed_point_id;
    uint32_t hdiv_cnt;          /**< Number of horizontal division lines */
    uint32_t vdiv_cnt;          /**< Number of vertical division lines */
    uint32_t point_cnt;         /**< Number of points in all series */
    lv_chart_type_t type  : 4;  /**< Chart type */
    lv_chart_update_mode_t update_mode : 2;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_CHART != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CHART_PRIVATE_H*/
