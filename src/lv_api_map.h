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
#include "lvgl/lvgl.h"

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


#if LV_USE_ARC


static inline void lv_arc_set_angles(lv_obj_t * arc, uint16_t start, uint16_t end)
{
    lv_arc_set_start_angle(arc, start);
    lv_arc_set_end_angle(arc, end);
}

#endif

#if LV_USE_CHART

#define lv_chart_get_point_cnt lv_chart_get_point_count

#endif


#if LV_USE_DDLIST

static inline void lv_ddlist_set_draw_arrow(lv_obj_t * ddlist, bool en)
{
    if(en) lv_ddlist_set_symbol(ddlist, LV_SYMBOL_DOWN);
    else lv_ddlist_set_symbol(ddlist, NULL);
}

static inline bool lv_ddlist_get_draw_arrow(lv_obj_t * ddlist)
{
    if(lv_ddlist_get_symbol(ddlist)) return true;
    else return false;
}

#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_API_MAP_H*/
