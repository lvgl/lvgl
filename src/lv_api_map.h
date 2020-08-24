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
 * V6.0 COMPATIBILITY
 *--------------------*/
#if LV_USE_API_EXTENSION_V6

static inline void lv_task_once(lv_task_t * task)
{
    lv_task_set_repeat_count(task, 1);
}

#if LV_USE_CHECKBOX

#define lv_checkbox_set_static_text lv_checkbox_set_text_static

#endif

#if LV_USE_CHART

#define lv_chart_get_point_cnt lv_chart_get_point_count

#endif


#if LV_USE_DROPDOWN

static inline void lv_dropdown_set_draw_arrow(lv_obj_t * ddlist, bool en)
{
    if(en) lv_dropdown_set_symbol(ddlist, LV_SYMBOL_DOWN);
    else lv_dropdown_set_symbol(ddlist, NULL);
}

static inline bool lv_dropdown_get_draw_arrow(lv_obj_t * ddlist)
{
    if(lv_dropdown_get_symbol(ddlist)) return true;
    else return false;
}

#define lv_dropdown_set_static_options lv_dropdown_set_options_static

#endif

#if LV_USE_BAR

/**
 * Make the bar symmetric to zero. The indicator will grow from zero instead of the minimum
 * position.
 * @param bar pointer to a bar object
 * @param en true: enable disable symmetric behavior; false: disable
 * @deprecated As of v7.0, you should use `lv_bar_set_type` instead.
 */
static inline void lv_bar_set_sym(lv_obj_t * bar, bool en)
{
    if(en)
        lv_bar_set_type(bar, LV_BAR_TYPE_SYMMETRICAL);
    else
        lv_bar_set_type(bar, LV_BAR_TYPE_NORMAL);
}

/**
 * Get whether the bar is symmetric or not.
 * @param bar pointer to a bar object
 * @return true: symmetric is enabled; false: disable
 * @deprecated As of v7.0, you should use `lv_bar_get_type` instead.
 */
static inline bool lv_bar_get_sym(lv_obj_t * bar)
{
    return lv_bar_get_type(bar) == LV_BAR_TYPE_SYMMETRICAL;
}

#endif

#if LV_USE_LABEL

#define lv_label_set_static_text lv_label_set_text_static

#endif

#if LV_USE_SLIDER

/**
 * Make the slider symmetric to zero. The indicator will grow from zero instead of the minimum
 * position.
 * @param slider pointer to a bar object
 * @param en true: enable disable symmetric behavior; false: disable
 * @deprecated As of v7.0, you should use `lv_slider_set_type` instead.
 */
static inline void lv_slider_set_sym(lv_obj_t * slider, bool en)
{
    lv_bar_set_sym(slider, en);
}

/**
 * Get whether the slider is symmetric or not.
 * @param slider pointer to a slider object
 * @return true: symmetric is enabled; false: disable
 * @deprecated As of v7.0, you should use `lv_slider_get_type` instead.
 */
static inline bool lv_slider_get_sym(lv_obj_t * slider)
{
    return lv_bar_get_sym(slider);
}

#endif

#if LV_USE_ROLLER

/**
 * Set a fixed width for the roller.
 * @param roller pointer to a roller object
 * @param w width
 * @deprecated As of v7.0, you should use `lv_roller_set_auto_fit` and set the width normally instead.
 */
static inline void lv_roller_set_fix_width(lv_obj_t * roller, lv_coord_t w)
{
    lv_roller_set_auto_fit(roller, false);
    lv_obj_set_width(roller, w);
}


#endif


#if LV_USE_PAGE
#define lv_scrlbar_mode_t lv_scrollbar_mode_t

#define LV_SCRLBAR_MODE_OFF    LV_SCROLLBAR_MODE_OFF
#define LV_SCRLBAR_MODE_ON     LV_SCROLLBAR_MODE_ON
#define LV_SCRLBAR_MODE_DRAG   LV_SCROLLBAR_MODE_DRAG
#define LV_SCRLBAR_MODE_AUTO   LV_SCROLLBAR_MODE_AUTO
#define LV_SCRLBAR_MODE_HIDE   LV_SCROLLBAR_MODE_HIDE
#define LV_SCRLBAR_MODE_UNHIDE LV_SCROLLBAR_MODE_UNHIDE


static inline void lv_page_set_scrlbar_mode(lv_obj_t * page, lv_scrlbar_mode_t sb_mode)
{
    lv_page_set_scrollbar_mode(page, sb_mode);
}
static inline lv_scrollbar_mode_t lv_page_get_scrlbar_mode(lv_obj_t * page)
{
    return lv_page_get_scrollbar_mode(page);
}


static inline lv_obj_t * lv_page_get_scrl(lv_obj_t * page)
{
    return lv_page_get_scrollable(page);
}
#endif


#endif /*LV_USE_API_EXTENSION_V6*/




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
    lv_obj_align_mid(obj, base, align, x_ofs, y_ofs);
}

static inline void lv_obj_align_origo_x(lv_obj_t * obj, const lv_obj_t * base, lv_align_t align, lv_coord_t x_ofs)
{
    lv_obj_align_mid_y(obj, base, align, x_ofs);
}

static inline void lv_obj_align_origo_y(lv_obj_t * obj, const lv_obj_t * base, lv_align_t align, lv_coord_t y_ofs)
{
    lv_obj_align_mid_y(obj, base, align, y_ofs);
}

#endif /*LV_USE_API_EXTENSION_V6*/
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_API_MAP_H*/
