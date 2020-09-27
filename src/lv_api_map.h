/**
 * @file lv_api_map.h
 *
 */

#ifndef LV_API_MAP_H
#define LV_API_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*---------------------
 * V7.0 COMPATIBILITY
 *--------------------*/
#if LV_USE_API_EXTENSION_V7
#if LV_USE_WIN

static inline lv_obj_t * lv_win_add_btn(lv_obj_t * win, const void * img_src)
{
    return lv_win_add_btn_right(win, img_src);
}

#endif

#if LV_USE_CHART
static inline void lv_chart_set_range(lv_obj_t * chart, lv_coord_t ymin, lv_coord_t ymax)
{
    lv_chart_set_y_range(chart, LV_CHART_AXIS_PRIMARY_Y, ymin,  ymax);
}


static inline void lv_chart_clear_serie(lv_obj_t * chart, lv_chart_series_t * series)
{
    lv_chart_clear_series(chart, series);
}

#endif

static inline void lv_obj_align_origo(lv_obj_t * obj, const lv_obj_t * base, lv_align_t align, lv_coord_t x_ofs,
                                      lv_coord_t y_ofs)
{
//    lv_obj_align_mid(obj, base, align, x_ofs, y_ofs);
}

static inline void lv_obj_align_origo_x(lv_obj_t * obj, const lv_obj_t * base, lv_align_t align, lv_coord_t x_ofs)
{
//    lv_obj_align_mid_y(obj, base, align, x_ofs);
}

static inline void lv_obj_align_origo_y(lv_obj_t * obj, const lv_obj_t * base, lv_align_t align, lv_coord_t y_ofs)
{
//    lv_obj_align_mid_y(obj, base, align, y_ofs);
}

#endif /*LV_USE_API_EXTENSION_V6*/
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_API_MAP_H*/
